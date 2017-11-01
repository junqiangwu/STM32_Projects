#include <arm11_ghs.h>
#include <armv7m_ghs.h>

#include <cortexm3_macro.h>

u32 __MRS_CONTROL(void)
{
    return __MRS(__CONTROL);
}

void __MSR_CONTROL(u32 Control)
{
    __MSR(__CONTROL, Control);
}

u32 __MRS_PSP(void)
{
    return __MRS(__PSP);
}

void __MSR_PSP(u32 TopOfProcessStack)
{
    __MSR(__PSP, TopOfProcessStack);
}

u32 __MRS_MSP(void)
{
    return __MRS(__MSP);
}

void __MSR_MSP(u32 TopOfMainStack)
{
    __MSR(__MSP, TopOfMainStack);
}

void __SETPRIMASK(void)
{
    __CPS(__CPS_ENABLE, __CPS_I, __CPS_DEFAULT);
}

void __RESETPRIMASK(void)
{
    __CPS(__CPS_DISABLE, __CPS_I, __CPS_DEFAULT);
}

void __SETFAULTMASK(void)
{
    __CPS(__CPS_ENABLE, __CPS_F, __CPS_DEFAULT);
}

void __RESETFAULTMASK(void)
{
    __CPS(__CPS_DISABLE, __CPS_F, __CPS_DEFAULT);
}

void __BASEPRICONFIG(u32 NewPriority)
{
    __MSR(__BASEPRI, NewPriority);
}

u32 __GetBASEPRI(void)
{
    return __MRS(__BASEPRI_MAX);
}

u16 __REV_HalfWord(u16 Data)
{
    return __REV16(Data);
}

u32 __REV_Word(u32 Data)
{
    return __REV(Data);
}
