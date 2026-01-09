#pragma once

/** 3D Data Class for C3DGrapher class.
*
*	This class is essentially CArray<CTurple, CTurple&> class
*
*	There are various methods here to make 3D data.
*/
template<class T>
class TC3DData
{
public:
	/** TC3DData object should be in the heap. So use this function to make TC3DData object
	*	@param		None
	*	@return		TC3DData<T>* - pointer of newly created TC3DData<T> object
	*	@exceptions	None
	*/
	static TC3DData<T>* GetInstance() throw()
	{
		return new TC3DData<T>;
	}
	/** clear internal vector
	*	@param		None
	*	@return		None
	*	@exceptions	None
	*/
	void Clear() throw()
	{
		m_strLegend = _T(""); m_vecData.clear();
	}
	/** Add new 3D data to the end of internal CArray data
	*	@param		xdata - x value which is to be added
	*	@param		ydata - y value which is to be added
	*	@param		zdata - z value which is to be added
	*	@return		None
	*	@exceptions	None
	*/
	void AddData(double xdata, double ydata, double zdata) throw()
	{
		m_vecData.push_back(Point3D(xdata, ydata, zdata));
	}
	/** Set values of internal vector data
	*	@param		index - where data is to be set
	*	@param		xdata - new x value
	*	@param		ydata - new y value
	*	@param		zdata - new z value
	*	@return		None
	*	@exceptions	None
	*/
	void SetAt(int index, double xdata, double ydata, double zdata) throw()
	{
		m_vecData[index].x = xdata; m_vecData[index].y = ydata; m_vecData[index].z = zdata;
	}
	/** Fix size of internal CArray data. Useful for faster allocation
	*	@param		size - new Size of 3D data
	*	@return		None
	*	@exceptions	None
	*/
	void SetSize(int size) throw()
	{
		m_vecData.resize(size);
	}
	/** Retrieve size of Internal vector
	*	@return		int - size
	*	@exceptions None
	*/
	int GetSize() const throw()
	{
		return m_vecData.size();
	}
	/** Retrieve xdata of given index
	*	@param		index - index value
	*	@retrurn	int - xdata value
	*	@exceptions None
	*/
	double GetXdata(int index) const throw()
	{
		return m_vecData[index].x;
	}
	/** Retrieve ydata of given index
	*	@param		index - index value
	*	@retrurn	int - ydata value
	*	@exceptions None
	*/
	double GetYdata(int index) const throw()
	{
		return m_vecData[index].y;
	}
	/** Retrieve zdata of given index
	*	@param		index - index value
	*	@retrurn	int - zdata value
	*	@exceptions None
	*/
	double GetZdata(int index) const throw()
	{
		return m_vecData[index].z;
	}

	void GetZMinMax(double& min, double& max)
	{
		vector<Point3D>::iterator iter;
		iter = max_element(m_vecData.begin(), m_vecData.end(), Point3DCompare);
		max = (*iter).z;
		iter = min_element(m_vecData.begin(), m_vecData.end(), Point3DCompare);
		min = (*iter).z;
	}
private:
	TC3DData(std::string legend = "Untitled") : m_strLegend(legend)
	{}
	TC3DData(const TC3DData<T>& rhs)
	{}
	TC3DData<T>& operator=(const TC3DData<T>& rhs)
	{}
	std::string	m_strLegend;
	vector<Point3D> m_vecData;
};

typedef TC3DData<double> C3DData;
