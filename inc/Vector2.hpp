namespace Magia
{
	template<class T>
	struct Vector2
	{
		T X, Y;

		Vector2(T x, T y)
			: X(x), Y(y)
		{ }
	};

	template<class T>
	constexpr Vector2<T> operator*(float f, Vector2<T> v)
	{
		v.X *= f;
		v.Y *= f;
		return v;
	}

	template<class T>
	constexpr Vector2<T> operator-(Vector2<T> v1, const Vector2<T>& v2)
	{
		v1.X -= v2.X;
		v1.Y -= v2.Y;
		return v1;
	}

	template<class T>
	constexpr Vector2<T> operator+(Vector2<T> v1, const Vector2<T>& v2)
	{
		v1.X += v2.X;
		v1.Y += v2.Y;
		return v1;
	}

	template<class T>
	constexpr float operator|(Vector2<T> v1, const Vector2<T>& v2)
	{
		float result = 0;
		result += v1.X * v2.X;
		result += v1.Y * v2.Y;
		return result;
	}
}