#pragma once

#include "ShaderFlat.h"

class CScene;

class CShaderPhong : public CShaderFlat
{
public:
	/**
	 * @brief Constructor
	 * @param scene Reference to the scene
	 * @param color The color of the object
	 * @param ka The ambient coefficient
	 * @param kd The diffuse reflection coefficients
	 * @param ks The specular refelection coefficients
	 * @param ke The shininess exponent
	 */
	CShaderPhong(CScene& scene, Vec3f color, float ka, float kd, float ks, float ke)
		: CShaderFlat(color)
		, m_scene(scene)
		, m_ka(ka)
		, m_kd(kd)
		, m_ks(ks)
		, m_ke(ke)
	{}
	virtual ~CShaderPhong(void) = default;

	virtual Vec3f Shade(const Ray& ray) const override
	{
		Vec3f sol_a;
		Vec3f diff_sum = 0;
		Vec3f spec_sum = 0;
		Ray ray2;
		Vec3f refl_dir;
		std::optional<Vec3f> intens;

		sol_a = m_ka*RGB(1,1,1);

		ray2.org = ray.org + ray.t*ray.dir;
		ray2.t = std::numeric_limits<float>::infinity();

		for (auto it = m_scene.m_vpLights.begin();it != m_scene.m_vpLights.end();++it)
		{
			intens = (*it).get()->Illuminate(ray2);
			if (!m_scene.Occluded(ray2))
			{
				diff_sum += *intens *max(0.0f,ray2.dir.dot(ray.hit->GetNormal(ray)));
				
				refl_dir = ray2.dir - 2 * (ray2.dir.dot(ray.hit->GetNormal(ray))) * ray.hit->GetNormal(ray);
				float dp = ray.dir.dot(refl_dir);
				if (dp<0) dp =0;
				spec_sum += *intens*pow(dp,m_ke);
			}	
		}

		Vec3f sol_d = m_kd*CShaderFlat::Shade(ray).mul(diff_sum);
		Vec3f sol_e = m_ks*RGB(1,1,1)*spec_sum;
		return sol_a + sol_d + sol_e;
	}

	
private:
	CScene& m_scene;
	float 	m_ka;    ///< ambient coefficient
	float 	m_kd;    ///< diffuse reflection coefficients
	float 	m_ks;    ///< specular refelection coefficients
	float 	m_ke;    ///< shininess exponent
};
