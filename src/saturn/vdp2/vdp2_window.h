static bool TestWindow(const u32&x, const u32&y, const Registers::WindowPosition &w, const Registers::WindowControl::WindowReg &reg)
{
	//these calculations aren't correct if the window is invalid.
	//todo research that and figure out the correct way to test

	if(reg.enable)
	{
		if(reg.outside)
		{
			if(x < w.x.start || x > w.x.end
				|| y < w.y.start || y > w.y.end)
				return true;//pixel is outside
		}
		else
		{
			//pixels outside window are visible
			if(x >= w.x.start && x <= w.x.end
				&& y >= w.y.start && y <= w.y.end
				)
				return true;//pixel is inside
		}
	}
	return false;
}

void ReadLineWindowValue(const u32 &address, Registers::WindowPosition & w)
{
	w.x.start = vdp2_ram.Read<kWord>(address);
	w.x.end = vdp2_ram.Read<kWord>(address+2);
}

struct WindowResult
{
	bool transparent;
	bool color_calculate;
	bool rotation_parameter;

	WindowResult() : transparent(false), color_calculate(false), rotation_parameter(false) {}
};

template <Layers layer, int which>
bool WindowReadAndTest(const u32&x, const u32&y)
{
	Registers::WindowPosition w = regs.window_position[which];

	u32 line_window_lead_address = regs.lwta[which].address;
	line_window_lead_address &= 0x7fffe;
	line_window_lead_address <<= 1;

	if(regs.lwta[which].enabled)
		ReadLineWindowValue(line_window_lead_address + (y * 4), w);

	//fixme figure out this sizing stuff
	if(!(regs.tvmd.hreso & (1 << 1)))
	{
		w.x.start >>= 1;
		w.x.end >>= 1;
	}

	return TestWindow(x,y,w,regs.window_control[layer].window[which]);
}

template <Layers layer>
WindowResult DoWindow(const u32&x, const u32&y, Registers&regs)
{
	WindowResult r;

	r.transparent = false;

	bool result[2];

	result[0] = WindowReadAndTest<layer,0>(x,y);
	result[1] = WindowReadAndTest<layer,1>(x,y);

	if(regs.window_control[layer].logic)//if and logic enabled
	{
		if(result[0] && result[1])
		{
			r.transparent = true;
		}
	}
	else
	{
		if(result[0] || result[1])//or logic
		{
			r.transparent = true;
		}
	}
	return r;
}