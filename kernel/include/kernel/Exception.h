
#ifndef _EXCEPTION_H
#define _EXCEPTION_H

namespace kernel
{
    class Exception
    {
    public:
        static void KernelPanic(const char* fmt, ...);
        static void DivideByZeroFault(unsigned int cs, unsigned int eip, unsigned int eflags);
        static void SingleStepTrap(unsigned int cs, unsigned int eip, unsigned int eflags);
        static void NmiTrap(unsigned int cs, unsigned int eip, unsigned int eflags);
        static void BreakpointTrap(unsigned int cs, unsigned int eip, unsigned int eflags);
        static void OverflowTrap(unsigned int cs, unsigned int eip, unsigned int eflags);
        static void BoundsCheckFault(unsigned int cs, unsigned int eip, unsigned int eflags);
        static void InvalidOpcodeFault(unsigned int cs, unsigned int eip, unsigned int eflags);
        static void NoDeviceFault(unsigned int cs, unsigned int eip, unsigned int eflags);
        static void DoubleFaultAbort(unsigned int cs, unsigned int err,unsigned int eip, unsigned int eflags);
        static void InvalidTssFault(unsigned int cs, unsigned int err,unsigned int eip, unsigned int eflags);
        static void NoSegmentFault(unsigned int cs, unsigned int err,unsigned int eip, unsigned int eflags);
        static void StackFault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);
        static void GeneralProtectionFault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);
        static void PageFault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);
        static void FpuFault(unsigned int cs, unsigned int eip, unsigned int eflags);
        static void AlignmentCheckFault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);
        static void MachineCheckAbort(unsigned int cs,  unsigned int eip, unsigned int eflags);
        static void SimdFpuFault(unsigned int cs, unsigned int eip, unsigned int eflags);
    };
}

#endif
