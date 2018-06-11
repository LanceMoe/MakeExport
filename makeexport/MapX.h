#ifndef MAPX_H
#define MAPX_H



#ifndef _MAP_
#include <map>
#endif

namespace XLIB
{
template<class _Kty,
	class _Ty,
	class _Pr = std::less<_Kty>,
	class _Alloc = std::allocator<std::pair<const _Kty, _Ty> > >
class CXMap:public std::map<_Kty,_Ty,_Pr,_Alloc>
{
public:
	typedef CXMap<_Kty, _Ty, _Pr, _Alloc> _Myt;
	typedef std::map<_Kty,_Ty,_Pr,_Alloc> _Mybase;

	bool Insert( _Kty key,_Ty value)//插入，返回成功与否
	{
		std::pair< _Myt::iterator,bool > pr;

		pr = _Mybase::insert(std::make_pair(key,value));

		return pr.second;
	}


	_Ty* Find( _Kty key,_Ty* pValue = 0 )//如果成功，返回value的指针,pValue中存放着值，如果失败，返回0
	{
		_Myt::iterator item;

		item = _Mybase::find( key );
		if( item == _Mybase::end() )return NULL;

		if( pValue )*pValue = item->second;
		return &item->second;
	}

};

}

#endif