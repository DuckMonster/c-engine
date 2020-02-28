Quat operator*(const Quat& a, const Quat& b)
{
	// b * a will concatenate the rotations
	// result is first (a), then (b)
	// (r1, v1)(r2, v2) = (r1r2 - v1.v2, r1v2 + r2v1 + v1 q.x v2)
	return Quat(
		b.w * a.x + b.x * a.w - b.y * a.z + b.z * a.y,
		b.w * a.y + b.x * a.z + b.y * a.w - b.z * a.x,
		b.w * a.z - b.x * a.y + b.y * a.x + b.z * a.w,
		b.w * a.w - b.x * a.x - b.y * a.y - b.z * a.z
	);
}

Vec3 operator*(const Quat& q, const Vec3& v)
{
	// http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
	// V' = V + 2w(Q x V) + (2Q x (Q x V))
	// refactor:
	// V' = V + w(2(Q x V)) + (Q x (2(Q x V)))
	// T = 2(Q x V);
	// V' = V + w*(T) + (Q x T)

	const Vec3 q_v(q.x, q.y, q.z);
	const Vec3 u = cross(q_v, v);
	const Vec3 a = cross(q_v, u) * 2.f;
	const Vec3 b = u * 2.f * q.w;

	return v + a + b;
}

Mat4 quat_to_mat(const Quat& q)
{
	// Copied from Unreal! Read up on this!
	Mat4 m;
	const float x2 = q.x + q.x;		const float y2 = q.y + q.y;		const float z2 = q.z + q.z;
	const float xx = q.x * x2;		const float xy = q.x * y2;		const float xz = q.x * z2;
	const float yy = q.y * y2;		const float yz = q.y * z2;		const float zz = q.z * z2;
	const float wx = q.w * x2;		const float wy = q.w * y2;		const float wz = q.w * z2;

	m.m00 = 1.0f - (yy + zz);	m.m10 = xy - wz;				m.m20 = xz + wy;			m.m30 = 0.f;
	m.m01 = xy + wz;			m.m11 = 1.0f - (xx + zz);		m.m21 = yz - wx;			m.m31 = 0.f;
	m.m02 = xz - wy;			m.m12 = yz + wx;				m.m22 = 1.0f - (xx + yy);	m.m32 = 0.f;
	m.m03 = 0.0f;				m.m13 = 0.0f;					m.m23 = 0.0f;				m.m33 = 1.0f;

	return m;
}

Quat mat_to_quat(const Mat4& m)
{
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	Quat q;

	// Check the diagonal
	float trace = m.m00 + m.m11 + m.m22;
	if (trace > 0.f)
	{
		float s = sqrtf(trace + 1.f) * 2.f;
		return Quat(
			(m.m12 - m.m21) / s,
			(m.m20 - m.m02) / s,
			(m.m01 - m.m10) / s,
			0.25f * s
		);
	}
	// If the diagonal is less than 0, find the axis of least rotation
	else if (m.m00 > m.m11 && m.m00 > m.m22)
	{
		float s = sqrtf(1.f + m.m00 - m.m11 - m.m22) * 2.f;
		return Quat(
			0.25f * s,
			(m.m10 + m.m01) / s,
			(m.m20 + m.m02) / s,
			(m.m12 - m.m21) / s
		);
	}
	else if (m.m11 > m.m22)
	{
		float s = sqrtf(1.f + m.m11 - m.m00 - m.m22) * 2.f;
		return Quat(
			(m.m10 + m.m01) / s,
			0.25f * s,
			(m.m21 + m.m12) / s,
			(m.m20 - m.m02) / s
		);
	}
	else
	{
		float s = sqrtf(1.f + m.m22 - m.m00 - m.m11) * 2.f;
		return Quat(
			(m.m20 + m.m02) / s,
			(m.m21 + m.m12) / s,
			0.25f * s,
			(m.m01 - m.m10) / s
		);
	}
}

Quat angle_axis(float angle, const Vec3& axis)
{
	float half_angle = angle / 2.f;
	return Quat(
		sin(half_angle) * axis.x,
		sin(half_angle) * axis.y,
		sin(half_angle) * axis.z,
		cos(half_angle)
	);
}

Quat quat_from_x(const Vec3& x)
{
	if (is_nearly_zero(x.x) && is_nearly_zero(x.y))
		return quat_from_xy(x, Vec3_Y);
	else
		return quat_from_xz(x, Vec3_Z);
}

Quat quat_from_xy(const Vec3& x, const Vec3& y)
{
	Vec3 axis_x = normalize(x);
	Vec3 axis_z = normalize(cross(x, y));
	Vec3 axis_y = cross(axis_z, axis_x);

	Mat4 mat(
		axis_x.x, axis_x.y, axis_x.z, 0.f,
		axis_y.x, axis_y.y, axis_y.z, 0.f,
		axis_z.x, axis_z.y, axis_z.z, 0.f,
		0.f, 0.f, 0.f, 1.f
	);

	return mat_to_quat(mat);
}

Quat quat_from_xz(const Vec3& x, const Vec3& z)
{
	Vec3 axis_x = normalize(x);
	Vec3 axis_y = normalize(cross(z, x));
	Vec3 axis_z = cross(axis_x, axis_y);

	Mat4 mat(
		axis_x.x, axis_x.y, axis_x.z, 0.f,
		axis_y.x, axis_y.y, axis_y.z, 0.f,
		axis_z.x, axis_z.y, axis_z.z, 0.f,
		0.f, 0.f, 0.f, 1.f
	);

	return mat_to_quat(mat);
}

Quat quat_from_euler(const Vec3& euler)
{
	return
		angle_axis(radians(euler.z), Vec3_Z) * 
		angle_axis(radians(euler.y), Vec3_Y) *
		angle_axis(radians(euler.x), Vec3_X);
}

Vec3 quat_x(const Quat& q)
{
	return q * Vec3_X;
}

Vec3 quat_y(const Quat& q)
{
	return q * Vec3_Y;
}

Vec3 quat_z(const Quat& q)
{
	return q * Vec3_Z;
}

Quat quat_from_to(const Vec3& from, const Vec3& to)
{
	if (is_nearly_equal(from, to))
		return Quat_Identity;
	if (is_nearly_zero(from) || is_nearly_zero(to))
		return Quat_Identity;

	Vec3 from_n = normalize(from);
	Vec3 to_n = normalize(to);

	Vec3 axis = cross(from_n, to_n);
	float angle = acos(dot(from_n, to_n));

	return angle_axis(angle, normalize(axis));
	//return Quat(axis.x, axis.y, axis.z, angle);
}