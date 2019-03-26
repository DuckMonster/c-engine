struct Quat
{
	Quat() : x(0.f), y(0.f), z(0.f), w(1.f) {}
	Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	float x;
	float y;
	float z;
	float w;
};

Quat operator*(const Quat& a, const Quat& b);
Mat4 quat_to_mat(const Quat& q);
Quat mat_to_quat(const Mat4& mat);
Quat angle_axis(float angle, const Vec3& axis);

Quat quat_from_x(const Vec3& x);