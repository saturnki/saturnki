using System;
using System.Windows.Forms;

using System.Drawing;
using System.Drawing.Imaging;

using SlimDX;
using SlimDX.Windows;
using SlimDX.Multimedia;
using SlimDX.Direct3D9;

using System.Runtime.InteropServices;

class MainWindow : System.Windows.Forms.Form
{
    void AudioUpdate(int l, int r) { }

    public void InitializeCallbacks()
    {
        Saturn.SetVideoUpdateCallback(Video.VideoRefresh);
        Saturn.SetAudioUpdateCallback(AudioUpdate);
        //todo

        Saturn.SetCdGetPhysicalStatusCallback(Cd.GetPhysicalStatus);
        Saturn.SetCdReadTocCallback(Cd.ReadToc);
        Saturn.SetCdReadSectorAtFadCallback(Cd.ReadSectorAtFad);
    }
}

static class Cd
{
    [DllImport("CdCore.dll", EntryPoint = "GetPhysicalStatus", CallingConvention = CallingConvention.Cdecl)]
    public static extern int GetPhysicalStatus();

    [DllImport("CdCore.dll", EntryPoint = "ReadToc", CallingConvention = CallingConvention.Cdecl)]
    public static extern uint ReadToc(IntPtr toc);

    [DllImport("CdCore.dll", EntryPoint = "ReadSectorAtFad", CallingConvention = CallingConvention.Cdecl)]
    public static extern int ReadSectorAtFad(uint fad, IntPtr buffer);

    [DllImport("CdCore.dll", EntryPoint = "CdInit", CallingConvention = CallingConvention.Cdecl)]
    public static extern int CdInit([MarshalAs(UnmanagedType.LPStr)] string filename);
}

//borrowed from byuu
static class Video
{
    static private IntPtr handle;

    static public Device device;
    static public VertexBuffer vertex;
    static public Texture texture;

    struct Vertex
    {
        public Vector4 Position;
        public Vector2 Offset;
    }

    public static void Initialize(IntPtr windowHandle)
    {
        handle = windowHandle;

        device = new Device(new Direct3D(), 0, DeviceType.Hardware, handle, CreateFlags.SoftwareVertexProcessing, new PresentParameters()
        {
            //PresentationInterval = PresentInterval.One,
            BackBufferWidth = 512,
            BackBufferHeight = 512
        });
        device.SetDialogBoxMode(false);

        device.SetTextureStageState(0, TextureStage.ColorOperation, TextureOperation.SelectArg1);
        device.SetTextureStageState(0, TextureStage.ColorArg1, TextureArgument.Texture);
        device.SetTextureStageState(0, TextureStage.ColorArg2, TextureArgument.Diffuse);

        device.SetTextureStageState(0, TextureStage.AlphaOperation, TextureOperation.SelectArg1);
        device.SetTextureStageState(0, TextureStage.AlphaArg1, TextureArgument.Texture);
        device.SetTextureStageState(0, TextureStage.AlphaArg2, TextureArgument.Diffuse);

        device.SetRenderState(RenderState.Lighting, false);
        device.SetRenderState(RenderState.ZEnable, false);
        device.SetRenderState(RenderState.CullMode, Cull.None);

        device.SetRenderState(RenderState.SourceBlend, Blend.SourceAlpha);
        device.SetRenderState(RenderState.DestinationBlend, Blend.InverseSourceAlpha);
        device.SetRenderState(RenderState.AlphaBlendEnable, false);

        device.SetSamplerState(0, SamplerState.MinFilter, TextureFilter.Linear);
        device.SetSamplerState(0, SamplerState.MagFilter, TextureFilter.Linear);

        device.VertexFormat = VertexFormat.PositionRhw | VertexFormat.Texture1;

        vertex = new VertexBuffer(device, 4 * 24, Usage.WriteOnly | Usage.Dynamic, VertexFormat.None, Pool.Default);
        texture = new Texture(device, 512, 512, 1, Usage.Dynamic, Format.X8R8G8B8, Pool.Default);

        device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Black, 1.0f, 0);
        device.Present();
    }

    public static void VideoRefresh(IntPtr data, int width, int height)
    {
        width = 704;
        height = 240;
        int pitch = 2816;

        SurfaceDescription desc = texture.GetLevelDescription(0);
        Surface surface = texture.GetSurfaceLevel(0);
        DataRectangle rectangle = surface.LockRectangle(SlimDX.Direct3D9.LockFlags.NoSystemLock | SlimDX.Direct3D9.LockFlags.Discard);
        unsafe
        {
            for (int ly = 0; ly < height; ly++)
            {
                int* output = (int*)rectangle.Data.DataPointer + ly * (rectangle.Pitch >> 2);
                short* input = (short*)data + ly * (pitch >> 1);
                for (int lx = 0; lx < width; lx++)
                {
                    short pixel = *input++;
                    *output++ = pixel;
                }
            }
        }
        surface.UnlockRectangle();
        surface.Dispose();

        device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Black, 1.0f, 0);
        device.BeginScene();

        float px = 0;
        float py = 0;
        float pw = width;
        float ph = height;

        float tw = 512;
        float th = 512;

        float ow = Program.main_window.ClientSize.Width;
        float oh = Program.main_window.ClientSize.Height;

        float ew = 512 * 54 / 47;
        float eh = 448;

        float mw = 1 * ow / ew;
        float mh = 1 * oh / eh;

        if (mw < mh)
        {
            ew *= mw;
            eh *= mw;
        }
        else
        {
            ew *= mh;
            eh *= mh;
        }

        ew *= 512.0f / ow;
        eh *= 512.0f / oh;

        float x = (512.0f - ew) / 2;
        float y = (512.0f - eh) / 2;
        float w = ew;
        float h = eh;

        float rw = w / pw * tw;
        float rh = h / ph * th;

        vertex.Lock(0, 0, SlimDX.Direct3D9.LockFlags.None).WriteRange(new[] {
      new Vertex() { Position = new Vector4(x     - 0.5f, y     - 0.5f, 0.0f, 1.0f), Offset = new Vector2((px    ) / rw, (py    ) / rh) },
      new Vertex() { Position = new Vector4(x + w - 0.5f, y     - 0.5f, 0.0f, 1.0f), Offset = new Vector2((px + w) / rw, (py    ) / rh) },
      new Vertex() { Position = new Vector4(x     - 0.5f, y + h - 0.5f, 0.0f, 1.0f), Offset = new Vector2((px    ) / rw, (py + h) / rh) },
      new Vertex() { Position = new Vector4(x + w - 0.5f, y + h - 0.5f, 0.0f, 1.0f), Offset = new Vector2((px + w) / rw, (py + h) / rh) }
    });
        vertex.Unlock();
        device.SetStreamSource(0, vertex, 0, 24);

        device.SetTexture(0, texture);
        device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);

        device.EndScene();
        device.Present();
    }
}

static class Program
{
    public static MainWindow main_window = new MainWindow();

    [STAThread]
    static void Main()
    {
        Application.EnableVisualStyles();
        Video.Initialize(main_window.Handle);
        main_window.InitializeCallbacks();
        main_window.Show();

        Cd.CdInit("C:\\SaturnOrbit\\EXAMPLES\\CharlesMacDonald\\satwin2\\main_coff.iso");

        Saturn.Power();

        while (main_window.Visible)
        {
            Saturn.RunFrame();
            Application.DoEvents();
        }
    }
}