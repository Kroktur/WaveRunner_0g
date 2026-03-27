#pragma once


class PhysicComponent
{
public:

	void uptadePhysique(float dt)
	{

		velocity += g * speed * dt;
		if (velocity >= max_velocity)
			velocity = max_velocity;
	}

	float getVelocity()
	{
		return velocity;
	}

	void setVelocity(float v)
	{
		velocity = v;
	}
	void resetVelocity()
	{
		velocity = 0.0f;
	}

	void jump()
	{
		if (m_isGround)
		{
			velocity -= jumpForce;
			m_isGround = false;
		}
	}

	void setIsGround(bool isGround)
	{
		m_isGround = isGround;
	}

	bool getIsGround()
	{
		return m_isGround;
	}

	void setjumpForce(float force)
	{
		jumpForce = force;
	}

private:
	float velocity = 0.0f;
	float g = 9.81f;
	float speed = 5.0f;
	float max_velocity = 10.0f;
	float jumpForce = 12.0f;
	bool m_isGround = false;
};