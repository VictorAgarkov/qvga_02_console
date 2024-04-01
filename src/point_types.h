#ifndef POINT_TYPES_H_INCLUDED
#define POINT_TYPES_H_INCLUDED

	//------------------------------------------------------------------------------
	struct point
	{
		short x, y;
	};
	//------------------------------------------------------------------------------
	union rect
	{
		struct
		{
			short x1, y1;
			short x2, y2;
		};
		unsigned long l[2];
		struct point p[2];
		short s[4];
	};
	//------------------------------------------------------------------------------


#endif /* POINT_TYPES_H_INCLUDED */
