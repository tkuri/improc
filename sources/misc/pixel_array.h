#ifndef PIG_ISP_PIXEL_ARRAY_H_
#define PIG_ISP_PIXEL_ARRAY_H_

#include <string>

namespace pol {
	enum PolarIndex {
		POLAR_0 = 0,  //!< 0[deg]
		POLAR_1 = 1,  //!< 45[deg]
		POLAR_2 = 2,  //!< 90[deg]
		POLAR_3 = 3,  //!< 135[deg]
		POLAR_NUM = 4 //!< The number of PolarIndex
	};

	/*! @brief �FIndex��` */
	enum ColorIndex {
		COLOR_R = 0,  //!< Red
		COLOR_G = 1,  //!< Green
		COLOR_B = 2,  //!< Blue
		COLOR_NUM = 3 //!< The number of ColorIndex
	};

	static inline int ConvertPolarColor(int polar, int color){
		return color + polar * COLOR_NUM;
	}

	template<int polar, int color>
	struct PolarColorTraits{
		static const int Index = color + polar * COLOR_NUM;
	};

	enum PolarColorIndex{
		POLAR_COLOR_R0 = PolarColorTraits<POLAR_0, COLOR_R>::Index, //!< Red 0[deg]
		POLAR_COLOR_G0 = PolarColorTraits<POLAR_0, COLOR_G>::Index, //!< Green 0[deg]
		POLAR_COLOR_B0 = PolarColorTraits<POLAR_0, COLOR_B>::Index, //!< Blue 0[deg]
		POLAR_COLOR_R1 = PolarColorTraits<POLAR_1, COLOR_R>::Index, //!< Red 45[deg]
		POLAR_COLOR_G1 = PolarColorTraits<POLAR_1, COLOR_G>::Index, //!< Green 45[deg]
		POLAR_COLOR_B1 = PolarColorTraits<POLAR_1, COLOR_B>::Index, //!< Blue 45[deg]
		POLAR_COLOR_R2 = PolarColorTraits<POLAR_2, COLOR_R>::Index, //!< Red 90[deg]
		POLAR_COLOR_G2 = PolarColorTraits<POLAR_2, COLOR_G>::Index, //!< Green 90[deg]
		POLAR_COLOR_B2 = PolarColorTraits<POLAR_2, COLOR_B>::Index, //!< Blue 90[deg]
		POLAR_COLOR_R3 = PolarColorTraits<POLAR_3, COLOR_R>::Index, //!< Red 135[deg]
		POLAR_COLOR_G3 = PolarColorTraits<POLAR_3, COLOR_G>::Index, //!< Green 135[deg]
		POLAR_COLOR_B3 = PolarColorTraits<POLAR_3, COLOR_B>::Index, //!< Blue 135[deg]
		POLAR_COLOR_NUM = 12
	};

	class PixelArray {
	public:

		/*! @brief
		@param [in] polar_phase(0-3)
		@param [in] color_phase(0-15)

		~~~
		0       | 1       | 2       | 3
		000 135 | 135 000 | 045 090 | 090 045
		045 090 | 090 045 | 000 135 | 135 000
		~~~
		0       | 1       | 2       | 3       | 4       | 5       | ... | 15
		R R G G | G R R G | G G R R | R G G R | G G B B | B G G B | ... | R G G R
		R R G G | G R R G | G G R R | R G G R | R R G G | G R R G | ... | G B B G
		G G B B | B G G B | B B G G | G B B G | R R G G | G R R G | ... | G B B G
		G G B B | B G G B | B B G G | G B B G | G G B B | B G G B | ... | R G G R
		~~~
		*/

		PixelArray(int polar_phase, int color_phase);

		void operator =(const PixelArray&) = delete; // non-copyable
		PixelArray(const PixelArray&) = delete; // non-copyable


		/*! @brief ���W(x,y)�̕Ό��p��Ԃ� */
		PolarIndex GetPolar(int x, int y);

		/*! @brief ���W(x,y)�̐F��Ԃ� */
		ColorIndex GetColor(int x, int y);

		/*! @brief ���W(x,y)�̕Ό��p�E�F��Ԃ� */
		PolarColorIndex GetPolarColor(int x, int y);

		bool IsR0(int x, int y); //!< @brief ���W(x,y)��Red 0[deg]�Ȃ�true.
		bool IsR1(int x, int y); //!< @brief ���W(x,y)��Red 45[deg]�Ȃ�true.
		bool IsR2(int x, int y); //!< @brief ���W(x,y)��Red 90[deg]�Ȃ�true.
		bool IsR3(int x, int y); //!< @brief ���W(x,y)��Red 135[deg]�Ȃ�true.
		bool IsG0(int x, int y); //!< @brief ���W(x,y)��Green 0[deg]�Ȃ�true.
		bool IsG1(int x, int y); //!< @brief ���W(x,y)��Green 45[deg]�Ȃ�true.
		bool IsG2(int x, int y); //!< @brief ���W(x,y)��Green 90[deg]�Ȃ�true.
		bool IsG3(int x, int y); //!< @brief ���W(x,y)��Green 135[deg]�Ȃ�true.
		bool IsB0(int x, int y); //!< @brief ���W(x,y)��Blue 0[deg]�Ȃ�true.
		bool IsB1(int x, int y); //!< @brief ���W(x,y)��Blue 45[deg]�Ȃ�true.
		bool IsB2(int x, int y); //!< @brief ���W(x,y)��Blue 90[deg]�Ȃ�true.
		bool IsB3(int x, int y); //!< @brief ���W(x,y)��Blue 135[deg]�Ȃ�true.

		bool IsR(int x, int y); //!< @brief ���W(x,y)��Red�Ȃ�true.
		bool IsG(int x, int y); //!< @brief ���W(x,y)��Green�Ȃ�true.
		bool IsB(int x, int y); //!< @brief ���W(x,y)��Blue�Ȃ�true.

		bool IsPolar0(int x, int y); //!< @brief ���W(x,y)��0[deg]�Ȃ�true.
		bool IsPolar1(int x, int y); //!< @brief ���W(x,y)��45[deg]�Ȃ�true.
		bool IsPolar2(int x, int y); //!< @brief ���W(x,y)��90[deg]�Ȃ�true.
		bool IsPolar3(int x, int y); //!< @brief ���W(x,y)��135[deg]�Ȃ�true.

		/*! @brief �z����𕶎���ɂ��ĕԂ�. */
		std::string ToString();

	private:
		static const int PERIOD_X_ = 4;
		static const int PERIOD_Y_ = 4;
		static const int MASK_X_ = PERIOD_X_ - 1;
		static const int MASK_Y_ = PERIOD_Y_ - 1;

		PolarIndex      polar_table_[PERIOD_X_ * PERIOD_Y_];
		ColorIndex      color_table_[PERIOD_X_ * PERIOD_Y_];
		PolarColorIndex polar_color_table_[PERIOD_X_ * PERIOD_Y_];
	};

} // namespace pol

#endif // 