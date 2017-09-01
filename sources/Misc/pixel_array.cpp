// [Confidential]
// Copyright 2017 Sony Corporation

/*! @file pig_isp_pixel_array.cpp */

#include <cassert>
#include "pixel_array.h"

namespace pol {

	PixelArray::PixelArray(int polar_phase, int color_phase) {

		assert(0 <= polar_phase && polar_phase < 4);
		assert(0 <= color_phase && color_phase < 16);
		static_assert(POLAR_COLOR_G2 == COLOR_G + POLAR_2 * COLOR_NUM, "Check the definition of PolarIndex, ColorIndex and PolarColorIndex.");

		const int pid_x = polar_phase % 2;
		const int pid_y = polar_phase / 2;
		const int cid_x = color_phase % 4;
		const int cid_y = color_phase / 4;

		for (int y = 0; y < PERIOD_Y_; ++y) {
			for (int x = 0; x < PERIOD_X_; ++x) {
				// polar phase
				//  0       | 1       | 2       | 3
				//  000 135 | 135 000 | 045 090 | 090 045
				//  045 090 | 090 045 | 000 135 | 135 000

				int px = (x % 2) ^ pid_x;
				int py = (y % 2) ^ pid_y;

				int p = px + (py << 1); // 0: 0[deg], 1: 135[deg], 2: 45[deg], 3: 90[deg]
				int polarIndex = (p == 0) ? POLAR_0 : (p == 1) ? POLAR_3 : (p == 2) ? POLAR_1 : POLAR_2; // 0: 0[deg], 1: 45[deg], 2: 90[deg], 3: 135[deg]
				// for hardware implement, following the process is equivalent
				//    int polarIndex = ((px ^ pid_x) << 1) + ((px ^ pid_x) ^ (py ^ pid_y)); // 0: 0[deg], 1: 45[deg], 2: 90[deg], 3: 135[deg]

				// color phase
				// 0       | 1       | 2       | 3       | 4       | 5       | ... | 15
				// R R G G | G R R G | G G R R | R G G R | G G B B | B G G B | ... | R G G R
				// R R G G | G R R G | G G R R | R G G R | R R G G | G R R G | ... | G B B G
				// G G B B | B G G B | B B G G | G B B G | R R G G | G R R G | ... | G B B G
				// G G B B | B G G B | B B G G | G B B G | G G B B | B G G B | ... | R G G R
				int cx = ((x - cid_x) & 0x3) / 2;
				int cy = ((y - cid_y) & 0x3) / 2;
				int c = cx + (cy << 1); // 0: R, 1: Gr, 2: Gb, 3:B

				int colorIndex = (c == 0) ? COLOR_R : (c == 1 || c == 2) ? COLOR_G : COLOR_B;

				polar_table_[x + y * PERIOD_X_] = static_cast<PolarIndex>(polarIndex);
				color_table_[x + y * PERIOD_X_] = static_cast<ColorIndex>(colorIndex);
				polar_color_table_[x + y * PERIOD_X_] = static_cast<PolarColorIndex>(colorIndex + polarIndex * COLOR_NUM); // follow the calculation of the enum PolarColorIndex{}
			}
		}
	}

	PolarIndex PixelArray::GetPolar(int x, int y) {
		return polar_table_[(x & MASK_X_) + (y & MASK_Y_) * PERIOD_X_];
	}

	ColorIndex PixelArray::GetColor(int x, int y) {
		return color_table_[(x & MASK_X_) + (y & MASK_Y_) * PERIOD_X_];
	}

	PolarColorIndex PixelArray::GetPolarColor(int x, int y) {
		return polar_color_table_[(x & MASK_X_) + (y & MASK_Y_) * PERIOD_X_];
	}

	bool PixelArray::IsR0(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_R0; }
	bool PixelArray::IsR1(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_R1; }
	bool PixelArray::IsR2(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_R2; }
	bool PixelArray::IsR3(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_R3; }
	bool PixelArray::IsG0(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_G0; }
	bool PixelArray::IsG1(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_G1; }
	bool PixelArray::IsG2(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_G2; }
	bool PixelArray::IsG3(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_G3; }
	bool PixelArray::IsB0(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_B0; }
	bool PixelArray::IsB1(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_B1; }
	bool PixelArray::IsB2(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_B2; }
	bool PixelArray::IsB3(int x, int y) { return GetPolarColor(x, y) == POLAR_COLOR_B3; }

	bool PixelArray::IsR(int x, int y) { return GetColor(x, y) == COLOR_R; }
	bool PixelArray::IsG(int x, int y) { return GetColor(x, y) == COLOR_G; }
	bool PixelArray::IsB(int x, int y) { return GetColor(x, y) == COLOR_B; }

	bool PixelArray::IsPolar0(int x, int y) { return GetPolar(x, y) == POLAR_0; }
	bool PixelArray::IsPolar1(int x, int y) { return GetPolar(x, y) == POLAR_1; }
	bool PixelArray::IsPolar2(int x, int y) { return GetPolar(x, y) == POLAR_2; }
	bool PixelArray::IsPolar3(int x, int y) { return GetPolar(x, y) == POLAR_3; }

	std::string PixelArray::ToString() {
		std::string tmp = "";
		for (int y = 0; y < PERIOD_Y_; ++y) {
			for (int x = 0; x < PERIOD_X_; ++x) {
				switch (color_table_[x + y * PERIOD_X_]){
				case COLOR_R: tmp += "R"; break;
				case COLOR_G: tmp += "G"; break;
				case COLOR_B: tmp += "B"; break;
				default: break;
				}

				switch (polar_table_[x + y * PERIOD_X_]) {
				case POLAR_0: tmp += "0"; break;
				case POLAR_1: tmp += "1"; break;
				case POLAR_2: tmp += "2"; break;
				case POLAR_3: tmp += "3"; break;
				default: break;
				}

				if (x != PERIOD_X_ - 1) {
					tmp += " ";
				}
			}
			if (y != PERIOD_Y_ - 1) {
				tmp += "\r\n";
			}
		}
		return tmp;
	}

} // namespace pig
