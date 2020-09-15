bool MathG::add_vvv(VectorG *a, VectorG *b, VectorG *r) noexcept
{
	if (!_ok || Index::_add_vvv == MG_ERROR_INDEX) return false;
	if (Index::_add_vvv == MG_UNINITED_INDEX)
	{
		const char *names[2] = { "a", "b" };
		ArgumentType types[2] = { ArgumentType::vector, ArgumentType::vector };
		Operation op;
		op.name = "add_vvv";
		op.argument_number = 2;
		op.argument_names = names;
		op.argument_types = types;
		op.result_type = ArgumentType::vector;
		op.check = [](Argument* arguments, ObjectG *result) -> bool
		{
			return(arguments[0].v->height() == arguments[1].v->height()
				&& arguments[0].v->height() == ((VectorG*)result)->height());
		};
		op.source =
			R"(
			#version 330 core
			uniform sampler2D a;
			uniform sampler2D b;
			out float c;
			void main()
			{
				int position = int(gl_FragCoord.y - 0.5);
				c = texelFetch(a, ivec2(0, position), 0).r + texelFetch(b, ivec2(0, position), 0).r;
			};
		)";
		Index::_add_vvv = submit(&op);
	}
	if (Index::_add_vvv == MG_ERROR_INDEX) return false;
	Argument arguments[2];
	arguments[0].v = a;
	arguments[1].v = b;
	return perform(Index::_add_vvv, arguments, r);
};

bool MathG::subtract_vvv(VectorG *a, VectorG *b, VectorG *r) noexcept
{
	if (!_ok || Index::_subtract_vvv == MG_ERROR_INDEX) return false;
	if (Index::_subtract_vvv == MG_UNINITED_INDEX)
	{
		const char *names[2] = { "a", "b" };
		ArgumentType types[2] = { ArgumentType::vector, ArgumentType::vector };
		Operation op;
		op.name = "subtract_vvv";
		op.argument_number = 2;
		op.argument_names = names;
		op.argument_types = types;
		op.result_type = ArgumentType::vector;
		op.check = [](Argument* arguments, ObjectG *result) -> bool
		{
			return(arguments[0].v->height() == arguments[1].v->height()
				&& arguments[0].v->height() == ((VectorG*)result)->height());
		};
		op.source =
			R"(
			#version 330 core
			uniform sampler2D a;
			uniform sampler2D b;
			out float c;
			void main()
			{
				int position = int(gl_FragCoord.y - 0.5);
				c = texelFetch(a, ivec2(0, position), 0).r - texelFetch(b, ivec2(0, position), 0).r;
			};
		)";
		Index::_subtract_vvv = submit(&op);
	}
	if (Index::_subtract_vvv == MG_ERROR_INDEX) return false;
	Argument arguments[2];
	arguments[0].v = a;
	arguments[1].v = b;
	return perform(Index::_subtract_vvv, arguments, r);
};

bool MathG::multiply_mvv(MatrixG *a, VectorG *b, VectorG *r) noexcept
{
	if (!_ok || Index::_multiply_mvv == MG_ERROR_INDEX) return false;
	if (Index::_multiply_mvv == MG_UNINITED_INDEX)
	{
		const char *names[3] = { "a", "b", "awidth" };
		ArgumentType types[3] = { ArgumentType::matrix, ArgumentType::vector, ArgumentType::int_ };
		Operation op;
		op.name = "multiply_mvv";
		op.argument_number = 3;
		op.argument_names = names;
		op.argument_types = types;
		op.result_type = ArgumentType::vector;
		op.check = [](Argument* arguments, ObjectG *result) -> bool
		{
			arguments[2].u = arguments[0].m->width();
			return(arguments[0].m->width() == arguments[1].v->height()
				&& arguments[0].m->height() == ((VectorG*)result)->height());
		};
		op.source =
			R"(
			#version 330 core

			uniform sampler2D a;
			uniform sampler2D b;
			uniform int awidth;

			out float c;

			void main()
			{
				int position = int(gl_FragCoord.y - 0.5);
				float sum = 0.0;
				for (int i = 0; i < awidth; i++)
				{
					sum += texelFetch(a, ivec2(i, position), 0).r * texelFetch(b, ivec2(0, position), 0).r;
				}
				c = sum;
			};
		)";
		Index::_multiply_mvv = submit(&op);
	}
	if (Index::_multiply_mvv == MG_ERROR_INDEX) return false;
	Argument arguments[3];
	arguments[0].m = a;
	arguments[1].v = b;
	return perform(Index::_multiply_mvv, arguments, r);
};