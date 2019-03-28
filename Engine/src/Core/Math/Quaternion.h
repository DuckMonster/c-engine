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
Vec3 operator*(const Quat& q, const Vec3& v);

Mat4 quat_to_mat(const Quat& q);
Quat mat_to_quat(const Mat4& mat);
Quat angle_axis(float angle, const Vec3& axis);

Quat quat_from_x(const Vec3& x);

Vec3 quat_x(const Quat& q);
Vec3 quat_y(const Quat& q);
Vec3 quat_z(const Quat& q);

const Quat Quat_Identity = Quat(0.f, 0.f, 0.f, 1.f);