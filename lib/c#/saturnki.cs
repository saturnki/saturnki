using System.Runtime.InteropServices;
using System;

class Saturn
{
    public static class Delegates
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void VideoUpdate(IntPtr data, int width, int height);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void AudioUpdate(int left, int right);

        //[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        //internal delegate void InputUpdateDelegate();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate int CdGetPhysicalStatus();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate uint CdReadToc(IntPtr toc);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate int CdReadSectorAtFad(uint fad, IntPtr buffer);
    }

    public static class SetCallback//delegate setting
    {

        [DllImport("SaturnCore.dll", EntryPoint = "SaturnSetVideoUpdateCallback", CallingConvention = CallingConvention.Cdecl)]
        public static extern void VideoUpdate(Delegates.VideoUpdate c);

        [DllImport("SaturnCore.dll", EntryPoint = "SaturnSetAudioSampleUpdateCallback", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AudioUpdate(Delegates.AudioUpdate c);

        //missing

        [DllImport("SaturnCore.dll", EntryPoint = "SaturnSetCdGetPhysicalStatusCallback", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CdGetPhysicalStatus(Delegates.CdGetPhysicalStatus c);

        [DllImport("SaturnCore.dll", EntryPoint = "SaturnSetCdReadTocCallback", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CdReadToc(Delegates.CdReadToc c);

        [DllImport("SaturnCore.dll", EntryPoint = "SaturnSetCdReadSectorAtFadCallback", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CdReadSectorAtFad(Delegates.CdReadSectorAtFad c);
    }

    //other

    [DllImport("SaturnCore.dll", EntryPoint = "SaturnPressPowerButton", CallingConvention = CallingConvention.Cdecl)]
    public static extern void Power();

    [DllImport("SaturnCore.dll", EntryPoint = "SaturnRunFrame", CallingConvention = CallingConvention.Cdecl)]
    public static extern void RunFrame();
}