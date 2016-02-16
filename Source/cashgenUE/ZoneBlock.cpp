
#include "cashgenUE.h"
#include "ZoneBlock.h"



ZoneBlock::ZoneBlock(float aHeight, FColor aColor)
{
	Height = aHeight;
	Color = aColor;
}



ZoneBlock::ZoneBlock()
{

}


float ZoneBlock::GetUpHeight()
{
	if (up) {
		return up->Height;
	}
	else {
		return this->Height;
	}
}
float ZoneBlock::GetDownHeight()
{
	if (down) {
		return down->Height;
	}
	else {
		return this->Height;
	}
}
float ZoneBlock::GetLeftHeight()
{
	if (left) {
		return left->Height;
	}
	else {
		return this->Height;
	}
}
float ZoneBlock::GetRightHeight()
{
	if (right) {
		return right->Height;
	}
	else {
		return this->Height;
	}
}

