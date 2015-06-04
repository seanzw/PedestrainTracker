function multiObjectTracking()

	obj = setupSystemObjects();
	tracks = initializeTracks();
	nextId = 1;

    global cemStartTime
    cemStartTime=tic;

    addpath(genpath('.'))
    global detMatrices detections sceneInfo opt globiter gtInfo
    globiter=0;

    %% setup options and scene
    % fill options struct
    opt=getConOptionsDemo;

    % fill scene info
    sceneInfo=getSceneInfoConDemo;

    %% cut ground truth
    gtInfo=cutGTToTrackingArea(gtInfo);

    %% load detections
    [detections nDets]=parseDetections(sceneInfo); fr=1:length(detections);
    stateInfo.F=size(detections,2);

    % cut detections to tracking area if needed
    [detections nDets]=cutDetections(detections, nDets);
    detMatrices=getDetectionMatrices(detections);

    curFrame = 1;

	while curFrame <= size(detMatrices.Xi, 1)

		frame = readFrame(curFrame);
		[centroids, bboxes] = detectObjects(curFrame);
		predictNewLocationsOfTracks();
		[assignments, unassignedTracks, unassignedDetections] = ...
			detectionToTrackAssignment();

		updateAssignedTracks();
		updateUnassignedTracks();
		deleteLostTracks();
		createNewTracks();

		displayTrackingResults();
        % displayDetectionResults(frame, curFrame);

        curFrame = curFrame + 1;

	end

	function obj = setupSystemObjects()
		% obj.reader = vision.VideoFileReader('1.avi');
		obj.videoPlayer = vision.VideoPlayer('Position', [20, 400, 700, 400]);
		obj.detector = vision.ForegroundDetector('NumGaussians', 3, ...
				'NumTrainingFrames', 40, 'MinimumBackgroundRatio', 0.7);
		obj.blobAnalyser = vision.BlobAnalysis('BoundingBoxOutputPort', true, ...
				'AreaOutputPort', true, 'CentroidOutputPort', true, ...
				'MinimumBlobArea', 400);
        obj.imgFormat = 'frame_%04d.jpg';
        obj.imgFolder = '../data/S3MF1/img/'
	end

	function tracks = initializeTracks()
        tracks = struct(...
            'id', {}, ...
            'bbox', {}, ...
            'kalmanFilter', {}, ...
            'age', {}, ...
            'totalVisibleCount', {}, ...
            'consecutiveInvisibleCount', {});
    end

    function frame = readFrame(curFrame)
        frame = imread([obj.imgFolder, sprintf(obj.imgFormat, curFrame)]);
    end

    function [centroids, bboxes] = detectObjects(curFrame)

        mask = (detMatrices.Xi(curFrame, :) ~= 0) & (detMatrices.H(curFrame, :) < 150);
        numDets = sum(mask);
        bboxes = zeros(numDets, 4);
        bboxes(:, 1) = detMatrices.Xi(curFrame, mask)' - detMatrices.W(curFrame, mask)' / 2;
        bboxes(:, 2) = detMatrices.Yi(curFrame, mask)' - detMatrices.H(curFrame, mask)';
        bboxes(:, 3) = detMatrices.W(curFrame, mask)';
        bboxes(:, 4) = detMatrices.H(curFrame, mask)';
        bboxes = int32(bboxes);

        centroids = zeros(numDets, 2);
        centroids(:, 1) = detMatrices.Xi(curFrame, mask)';
        centroids(:, 2) = detMatrices.Yi(curFrame, mask)' - detMatrices.H(curFrame, mask)' / 2;

    end

    function predictNewLocationsOfTracks()
        for i = 1:length(tracks)
            bbox = tracks(i).bbox;

            % Predict the current location of the track.
            predictedCentroid = predict(tracks(i).kalmanFilter);

            % Shift the bounding box so that its center is at
            % the predicted location.
            predictedCentroid = int32(predictedCentroid) - bbox(3:4) / 2;
            tracks(i).bbox = [predictedCentroid, bbox(3:4)];
        end
    end

    function [assignments, unassignedTracks, unassignedDetections] = ...
            detectionToTrackAssignment()

        nTracks = length(tracks);
        nDetections = size(centroids, 1);

        % Compute the cost of assigning each detection to each track.
        cost = zeros(nTracks, nDetections);
        for i = 1:nTracks
            cost(i, :) = distance(tracks(i).kalmanFilter, centroids);
        end

        % Solve the assignment problem.
        costOfNonAssignment = 20;
        [assignments, unassignedTracks, unassignedDetections] = ...
            assignDetectionsToTracks(cost, costOfNonAssignment);
    end

    function updateAssignedTracks()
        numAssignedTracks = size(assignments, 1);
        for i = 1:numAssignedTracks
            trackIdx = assignments(i, 1);
            detectionIdx = assignments(i, 2);
            centroid = centroids(detectionIdx, :);
            bbox = bboxes(detectionIdx, :);

            % Correct the estimate of the object's location
            % using the new detection.
            correct(tracks(trackIdx).kalmanFilter, centroid);

            % Replace predicted bounding box with detected
            % bounding box.
            tracks(trackIdx).bbox = bbox;

            % Update track's age.
            tracks(trackIdx).age = tracks(trackIdx).age + 1;

            % Update visibility.
            tracks(trackIdx).totalVisibleCount = ...
                tracks(trackIdx).totalVisibleCount + 1;
            tracks(trackIdx).consecutiveInvisibleCount = 0;
        end
    end

    function updateUnassignedTracks()
        for i = 1:length(unassignedTracks)
            ind = unassignedTracks(i);
            tracks(ind).age = tracks(ind).age + 1;
            tracks(ind).consecutiveInvisibleCount = ...
                tracks(ind).consecutiveInvisibleCount + 1;
        end
    end

    function deleteLostTracks()
        if isempty(tracks)
            return;
        end

        invisibleForTooLong = 20;
        ageThreshold = 8;

        % Compute the fraction of the track's age for which it was visible.
        ages = [tracks(:).age];
        totalVisibleCounts = [tracks(:).totalVisibleCount];
        visibility = totalVisibleCounts ./ ages;

        % Find the indices of 'lost' tracks.
        lostInds = (ages < ageThreshold & visibility < 0.6) | ...
            [tracks(:).consecutiveInvisibleCount] >= invisibleForTooLong;

        % Delete lost tracks.
        tracks = tracks(~lostInds);
    end

    function createNewTracks()
        centroids = centroids(unassignedDetections, :);
        bboxes = bboxes(unassignedDetections, :);

        for i = 1:size(centroids, 1)

            centroid = centroids(i,:);
            bbox = bboxes(i, :);

            % Create a Kalman filter object.
            kalmanFilter = configureKalmanFilter('ConstantVelocity', ...
                centroid, [200, 50], [100, 25], 100);

            % Create a new track.
            newTrack = struct(...
                'id', nextId, ...
                'bbox', bbox, ...
                'kalmanFilter', kalmanFilter, ...
                'age', 1, ...
                'totalVisibleCount', 1, ...
                'consecutiveInvisibleCount', 0);

            % Add it to the array of tracks.
            tracks(end + 1) = newTrack;

            % Increment the next id.
            nextId = nextId + 1;
        end
    end

    function displayTrackingResults()
        % Convert the frame and the mask to uint8 RGB.
        frame = im2uint8(frame);

        minVisibleCount = 8;
        if ~isempty(tracks)

            % Noisy detections tend to result in short-lived tracks.
            % Only display tracks that have been visible for more than
            % a minimum number of frames.
            reliableTrackInds = ...
                [tracks(:).totalVisibleCount] > minVisibleCount;
            reliableTracks = tracks(reliableTrackInds);

            % Display the objects. If an object has not been detected
            % in this frame, display its predicted bounding box.
            if ~isempty(reliableTracks)
                % Get bounding boxes.
                bboxes = cat(1, reliableTracks.bbox);

                % Get ids.
                ids = int32([reliableTracks(:).id]);

                % Create labels for objects indicating the ones for
                % which we display the predicted rather than the actual
                % location.
                labels = cellstr(int2str(ids'));
                predictedTrackInds = ...
                    [reliableTracks(:).consecutiveInvisibleCount] > 0;
                isPredicted = cell(size(labels));
                isPredicted(predictedTrackInds) = {' predicted'};
                labels = strcat(labels, isPredicted);

                % Draw the objects on the frame.

                frame = insertObjectAnnotation(frame, 'rectangle', ...
                    bboxes, labels);


            end
        end

        obj.videoPlayer.step(frame);
    end

    function displayDetectionResults(frame, curFrame)

        % Convert the frame and the mask to uint8 RGB.
        frame = im2uint8(frame);

        % Use some trick to ignore some detections.
        mask = (detMatrices.Xi(curFrame, :) ~= 0) & (detMatrices.H(curFrame, :) < 150);
        numDets = sum(mask);
        bboxes = zeros(numDets, 4);
        bboxes(:, 1) = detMatrices.Xi(curFrame, mask)' - detMatrices.W(curFrame, mask)' / 2;
        bboxes(:, 2) = detMatrices.Yi(curFrame, mask)' - detMatrices.H(curFrame, mask)';
        bboxes(:, 3) = detMatrices.W(curFrame, mask)';
        bboxes(:, 4) = detMatrices.H(curFrame, mask)';

        labels = 1 : numDets;
        frame = insertObjectAnnotation(frame, 'rectangle', ...
                    bboxes, labels);
        obj.videoPlayer.step(frame);

    end

end