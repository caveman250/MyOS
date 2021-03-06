#pragma once

namespace kernel
{
    //just some basic printf tests for the time being.
    class KernelTests
    {
    public:
        static void RunTest(const char* testName);
    private:
        static void MemoryMap();
        static void Paging();
        static void Allocations();
        static void SoftwareInterrupt();
        static void ReadFloppyDiskSector();
        static void ShowTestHelpMessage();
    };
}