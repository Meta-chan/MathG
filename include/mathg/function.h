/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_FUNCTION
#define MATHG_FUNCTION

#include "mathg.h"
#include "type.h"
#include <vector>
#include <string>

namespace mathg
{
	///Function to be performed on matrixes
	class Function
	{
	friend Matrix;
	private:
		struct Output
		{
			Type typ				= Type::int_;
			GLuint width_location	= MathG::_error;
			GLuint height_location	= MathG::_error;
			std::string name;
		};

		struct Uniform : Output
		{
			GLuint location			= MathG::_error;
			bool matrix				= false; 
		};

		struct Preprocessor
		{
			size_t i				= 0;
			std::string source;
			size_t local_level		= 0;
			size_t local_start		= 0;
			bool local_column_used	= false;
			bool local_row_used		= false;
			bool uses_double		= false;
		};

		std::vector<Uniform> _uniforms;
		Output _output;
		GLuint _program = MathG::_error;
		
		static bool _idcmp(const char *str1, const char *str2)	noexcept;
		static size_t _idlen(const char *str1)					noexcept;
		static size_t _spacelen(const char *str1)				noexcept;
		Uniform *_idmatrix(const char *str1)					noexcept;
		
		void _row(Preprocessor *p);
		void _column(Preprocessor *p);
		void _matrix_name_dot(Preprocessor *p, Uniform *matrix, size_t replace_start);
		void _matrix_name_bracket(Preprocessor *p, Uniform *matrix, size_t replace_start);
		void _matrix_name(Preprocessor *p, Uniform *matrix);
		void _output_name(Preprocessor *p);

		void _matrix(Preprocessor *p);
		void _uniform(Preprocessor *p);
		void _out(Preprocessor *p);

		Function(const Function &function) noexcept;
		
	public:
		///Creates empty function
		Function()																	noexcept;
		///Creates function from source
		///@param source Source of function
		///@param error Buffer to print error
		Function(const char *source, char *error = nullptr, GLsizei error_size = 0)	noexcept;
		///Initializes existing function
		///@param source Source of function
		///@param error Buffer to print error
		bool init(const char *source, char *error = nullptr, GLsizei error_size = 0)noexcept;
		///Returns if function is ok
		bool ok()																	const noexcept;
		///Finalizes function
		void finalize()																noexcept;
		///Destroys function
		~Function()																	noexcept;
	};
}

#endif	//#ifndef MATHG_FUNCTION

#ifdef MATHG_INCLUDE
	#ifndef MATHG_FUNCTION_SOURCE
		#define MATHG_FUNCTION_SOURCE
		#include "../../source/function.h"
	#endif
#endif