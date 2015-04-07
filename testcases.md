## Test Cases
# Valid Results
Test #		Input						Output
------		---------------------------------------		----------------------------
1		r=undef,	g=undef,	b=undef		-EINVAL
2		r=0,		g=0,		b=0		LED off
3		r=2047,		g=2047,		b=2047		LED white
4		r=2047,		g=0		b=0		LED red
5		r=0,		g=2047,		b=0		LED green
6		r=0,		g=0,		b=2047		LED blue
7		r=-1,		g=-1,		b=-1		-EINVAL
8		r=-1,		g=0,		b=0		-EINVAL
9		r=0,		g=-1,		b=0		-EINVAL
10		r=0,		g=0,		b=-1		-EINVAL
11		r=2048,		g=2048,		b=2048		-EINVAL
12		r=2048,		g=2047,		b=2047		-EINVAL
13		r=2047,		g=2048,		b=2047		-EINVAL
14		r=2047,		g=2047,		b=2048		-EINVAL
15		open device as r/w				-EINVAL
16		open device as read-only			-EINVAL
