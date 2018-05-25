//
// Created by KingSun on 2018/4/22.
//

#ifndef FUNCTION_H
#define FUNCTION_H

#include "../Header.h"

//Some functions
namespace KFunction {
	//Fundamental functions
	template <typename T>
	T abs(const T &c) {
		return c >= static_cast<T>(0) ? c : -c;
	}
	template <typename T>
	Kboolean isZero(const T &c) {
		return abs(c) <= static_cast<T>(EPSILON_E6);
	}
	template <typename T>
	const T& min(const T &a, const T &b) {
		return a < b ? a : b;
	}
	template <typename T>
	const T& max(const T &a, const T &b) {
		return a > b ? a : b;
	}
	template <typename T>
	T clamp(const T &val, const T &min, const T &max) {
		if (max < min) return static_cast<T>(KNAN);
		if (val > max) return max;
		if (val < min) return min;
		return val;
	}
	template <typename T, typename F = Kdouble>
	F toDegree(const T &radian) {
		return F(radian) * static_cast<F>(180) / static_cast<F>(PI);
	}
	template <typename T, typename F = Kdouble>
	F toRadian(const T &degree) {
		return F(degree) * static_cast<F>(PI) / static_cast<F>(180);
	}

	//vector functions
	template <typename vecType, typename F = Kdouble>
	F dot(const vecType &vec1, const vecType &vec2) {
		F res = 0;
		for (int i = 0; i < vec1.dimension(); ++i) res += vec1[i] * vec2[i];
		return res;
	};

	template <typename vecType, typename F = Kdouble>
	F length(const vecType &vec) {
		return sqrt(dot<vecType, F>(vec, vec));
	};

	template <typename vecType, typename F = Kdouble>
	F distance(const vecType &vec1, const vecType &vec2) {
		return length<vecType, F>(vec2 - vec1);
	};

	template <typename vecType, typename F = Kdouble>
	vecType normalize(const vecType &vec) {
		const F len = length<vecType, F>(vec);
		if (isZero(len)) return vecType(KNAN);
		return vec / len;
	};

	template <typename vecType, typename F = Kdouble>
	vecType faceforward(const vecType &N, const vecType &I, const vecType Nref) {
		if (dot<vecType, F>(Nref, I) < static_cast<F>(0)) {
			return N;
		}
		else {
			return -N;
		}
	};

	template <typename vecType>
	vecType reflect(const vecType &I, const vecType &N) {
		const vecType n = normalize(N);
		return I - static_cast<Kfloat>(2) * dot<vecType, Kfloat>(n, I) * n;
	};

	template <typename vecType, typename F = Kdouble>
	vecType refract(const vecType &I, const vecType &N, const F &eta) {
		const vecType i = normalize(I);
		const vecType n = normalize(N);
		const F cosni = dot<vecType, F>(n, i);

		const F k = static_cast<F>(1) - eta * eta * (static_cast<F>(1) - cosni * cosni);
		if (k < static_cast<F>(0)) {
			return vecType();
		}
		else {
			return eta * i - (eta * cosni + sqrt(k)) * n;
		}
	};

	//matrix functions
	template <typename matType>
	matType inverse(const matType &m) {
		return matType(m).inverse();
	};

	template <typename matType>
	matType transpose(const matType &m) {
		return matType(m).inverse();
	};

	template <typename matType, typename F = Kdouble>
	F determinant(const matType &m) {
		return m.determinant();
	};
}

#endif //FUNCTION_H
