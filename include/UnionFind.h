/**
 * A weighted union find with path compression.
 * @author Zhengrong Wang
 */

#ifndef UNION_FIND_HEADER
#define UNION_FIND_HEADER

class UnionFind {
public:
	UnionFind(int N);
	~UnionFind();

	/**
	 * Returns the number of components.
	 * @return the number of components (between 1 and N)
	 */
	int GetCount() const { return this->count; }
	
	/**
     * Returns the component identifier for the component containing site <tt>p</tt>.
     * @param p the integer representing one site
     * @return the component identifier for the component containing site <tt>p</tt>
     * @throws java.lang.IndexOutOfBoundsException unless 0 <= p < N
     */
	int Find(int p) const;

	/**
     * Are the two sites <tt>p</tt> and <tt>q</tt> in the same component?
     * @param p the integer representing one site
     * @param q the integer representing the other site
     * @return <tt>true</tt> if the two sites <tt>p</tt> and <tt>q</tt>
     *    are in the same component, and <tt>false</tt> otherwise
     * @throws java.lang.IndexOutOfBoundsException unless both 0 <= p < N and 0 <= q < N
     */
	bool Connected(int p, int q) const { return Find(p) == Find(q); }

	/**
     * Merges the component containing site<tt>p</tt> with the component
     * containing site <tt>q</tt>.
     * @param p the integer representing one site
     * @param q the integer representing the other site
     * @throws java.lang.IndexOutOfBoundsException unless both 0 <= p < N and 0 <= q < N
     */
	void Union(int p, int q);

private:
	int *parent;
	int *size;
	int count;
	int num;

	// Is this a valid index?
	inline bool Validate(int p) const;
};

#endif