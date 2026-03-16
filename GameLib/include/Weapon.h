#pragma once
#include <glm/glm.hpp>
#include "Core/Window.h"
#include "Core/Mesh.h"
#include "Core/Texture.h"
#include "Core/TrasformComponent.h"
#include "ECS/Registry.h"
#include "CollisionComponent.h"

/**
 * @brief Component representing a limited lifetime for an entity.
 *
 * Used for temporary objects such as bullets, particles, or effects.
 */
struct LivingTimeComponent
{
    float timeToLive; ///< Remaining lifetime in seconds.

    /// @brief Decreases lifetime by delta time.
    void Update(float dt)
    {
        timeToLive -= dt;
    }

    /// @brief Returns true if the entity should be destroyed.
    bool IsExpired() const
    {
        return timeToLive <= 0.0f;
    }
};

namespace KGR
{
    namespace GameLib
    {
        /**
         * @brief Types of weapons available in the game.
         */
        enum class WeaponType
        {
            Shotgun,
            Auto,
            Sniper
        };

        /**
         * @brief Data describing a weapon's behavior and stats.
         *
         * Includes:
         * - damage
         * - fire rate
         * - range
         * - spread (shotgun)
         * - ammo capacity
         * - reload time
         */
        struct WeaponData
        {
            float damage;
            float fireRate;   ///< Time between shots.
            float range;
            float spread;     ///< Shot dispersion (used for shotgun).
            int   maxAmmo;
            float reloadTime;
        };

        /**
         * @brief Component representing a bullet projectile.
         *
         * Contains:
         * - direction
         * - speed
         * - lifetime
         * - damage
         */
        struct BulletComponent
        {
            glm::vec3 direction; ///< Normalized movement direction.
            float speed = 50.0f; ///< Movement speed.
            float lifetime = 2.0f; ///< Time before destruction.
            float damage = 10.0f; ///< Damage dealt on hit.
        };

        /**
         * @brief Component managing weapon behavior, ammo, and firing logic.
         *
         * Stores:
         * - current weapon type
         * - weapon stats for each type
         * - ammo count
         * - cooldown and reload state
         */
        struct WeaponComponent
        {
            WeaponType current = WeaponType::Auto;

            WeaponData shotgun{ 50.0f, 1.0f, 15.0f, 0.15f, 8,  2.0f };
            WeaponData autoRifle{ 5.0f,  0.2f, 30.0f, 0.05f, 30, 1.5f };
            WeaponData sniper{ 200.0f,1.5f,100.0f, 0.0f,  5,  3.0f };

            float cooldown = 0.0f; ///< Time until next shot.
            int currentAmmo = 30;  ///< Current ammo count.
            bool isReloading = false;
            float reloadTimer = 0.0f;

            /**
             * @brief Returns the WeaponData of the currently equipped weapon.
             */
            const WeaponData& GetCurrentWeaponData() const;

            /**
             * @brief Switches to a new weapon type.
             */
            void SwitchWeapon(WeaponType newType);

            /**
             * @brief Spawns a bullet entity with mesh, transform, texture, collider, and lifetime.
             *
             * @tparam Registry ECS registry type.
             * @param registry ECS registry instance.
             * @param window Render window for resource loading.
             * @param position Spawn position.
             * @param direction Normalized direction of travel.
             */
            template<typename Registry>
            void CreateBullet(Registry& registry,
                KGR::RenderWindow& window,
                const glm::vec3& position,
                const glm::vec3& direction);

            /**
             * @brief Starts the reload process.
             */
            void StartReload();

            /**
             * @brief Updates reload timer and completes reload when finished.
             */
            void UpdateReload(float dt);

            /**
             * @brief Returns the weapon name as a string (for HUD).
             */
            std::string GetCurrentWeaponName() const;
        };


        // ---------------------------------------------------------------------
        // Template Implementation
        // ---------------------------------------------------------------------

		template<typename Registry>
		void WeaponComponent::CreateBullet(Registry& registry, KGR::RenderWindow& window, const glm::vec3& position, const glm::vec3& direction)
		{
			auto bullet = registry.CreateEntity();

			//Mesh
			MeshComponent meshComp;
			meshComp.mesh = &MeshLoader::Load("Models\\CUBE.obj", window.App());

			//Transform
			TransformComponent transform;
			transform.SetScale({ 0.1f, 0.1f, 0.1f });
			transform.SetPosition(position);
			transform.LookAtDir(direction);

			//Texture
			TextureComponent texture;
			texture.SetSize(meshComp.mesh->GetSubMeshesCount());
			for (int i = 0; i < meshComp.mesh->GetSubMeshesCount(); ++i)
				texture.AddTexture(i, &TextureLoader::Load("Textures\\BaseTexture.png", window.App()));

			CollisionComp collider;
			collider.collider = &ColliderManager::Load("bulletCollider", meshComp.mesh);

			//Damage and direction
			BulletComponent bulletComp;
			bulletComp.direction = direction;
			bulletComp.damage = GetCurrentWeaponData().damage;
			bulletComp.speed = 50.0f;
			bulletComp.lifetime = 2.0f;

			LivingTimeComponent timeToLive;
			timeToLive.timeToLive = bulletComp.lifetime;

			registry.AddComponents<MeshComponent, TransformComponent, TextureComponent, BulletComponent, LivingTimeComponent, CollisionComp>
				(bullet, std::move(meshComp), std::move(transform), std::move(texture), std::move(bulletComp), std::move(timeToLive), std::move(collider));
		}
	}
}