

#include "UnrealLibNoise.h"
#include "Const.h"

/// Constructor.
///
/// The default constant value is set to
/// noise::module::DEFAULT_CONST_VALUE.
UConst::UConst(const FObjectInitializer& ObjectInit) : Super(ObjectInit) { ConstValue = DEFAULT_CONST_VALUE; }