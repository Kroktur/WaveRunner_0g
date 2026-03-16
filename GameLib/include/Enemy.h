#pragma once
#include <glm/glm.hpp>
#include "Core/Window.h"
#include "Core/Mesh.h"
#include "Core/Texture.h"
#include "Core/TrasformComponent.h"
#include "ECS/Registry.h"
#include "CollisionComponent.h"

namespace KGR
{
    namespace GameLib
    {
        /**
         * @brief Component storing gameplay attributes for an enemy entity.
         *
         * Contains basic combat and movement parameters:
         * - movement speed
         * - attack range and cooldown
         * - damage dealt
         * - health
         */
        struct EnemyComponent
        {
            float speed;              ///< Movement speed.
            float attackRange;        ///< Distance required to attack.
            float damage;             ///< Damage dealt per attack.
            float health;             ///< Current health.
            float attackCooldown;     ///< Time between attacks.
            float timeSinceLastAttack;///< Time elapsed since last attack.
        };

        /**
         * @brief AI logic component for autonomous enemy behavior.
         *
         * Stores a target position and provides:
         * - target updates
         * - movement steering toward the target
         * - random position generation
         * - enemy spawning utility
         */
        struct AIComponent
        {
            /**
             * @brief Updates the AI target position.
             *
             * @param target New target world position.
             */
            void UpdateTarget(const glm::vec3& target);

            /**
             * @brief Computes movement direction toward the target.
             *
             * @param dt Delta time.
             * @param pos Current position.
             * @param speed Movement speed.
             * @return New position or movement vector.
             */
            glm::vec3 Update(float dt, const glm::vec3& pos, float speed);

            /**
             * @brief Generates a random point within a spherical range.
             *
             * @param center Center of the range.
             * @param minR Minimum radius.
             * @param maxR Maximum radius.
             * @return Random point in world space.
             */
            glm::vec3 RandomRange(const glm::vec3& center, float minR, float maxR);

            /**
             * @brief Spawns a fully configured enemy entity.
             *
             * Creates:
             * - MeshComponent (enemy model)
             * - TransformComponent (random spawn around player)
             * - TextureComponent (default texture)
             * - CollisionComp (AABB collider)
             * - AIComponent (target set to player)
             * - EnemyComponent (default stats)
             *
             * @tparam Registry ECS registry type.
             * @param registry ECS registry instance.
             * @param window Render window for resource loading.
             * @param playerPos Player world position (used for spawn radius).
             */
            template<typename Registry>
            void SpawnEnemy(Registry& registry, KGR::RenderWindow& window, const glm::vec3& playerPos);

        private:
            glm::vec3 m_target; ///< Current AI target position.
        };


        // ---------------------------------------------------------------------
        // Template Implementation
        // ---------------------------------------------------------------------

        template<typename Registry>
        void KGR::GameLib::AIComponent::SpawnEnemy(Registry& registry,
            KGR::RenderWindow& window,
            const glm::vec3& playerPos)
        {
            auto enemy = registry.CreateEntity();

            // Mesh
            MeshComponent meshComp;
            meshComp.mesh = &MeshLoader::Load("Models\\noe_renegade.obj", window.App());

            // Transform
            TransformComponent transform;
            transform.SetPosition(RandomRange(playerPos, 15.0f, 20.0f));
            transform.SetScale({ 1.0f, 1.0f, 1.0f });

            // Texture
            TextureComponent texture;
            texture.SetSize(meshComp.mesh->GetSubMeshesCount());
            for (int i = 0; i < meshComp.mesh->GetSubMeshesCount(); ++i)
                texture.AddTexture(i, &TextureLoader::Load("Textures\\basecolor.png", window.App()));

            // Collider
            CollisionComp collider;
            collider.collider = &ColliderManager::Load("enemyCollider", meshComp.mesh);

            // AI
            AIComponent ai;
            ai.UpdateTarget(playerPos);

            // Enemy stats
            EnemyComponent enemyComp;
            enemyComp.speed = 0.5f;
            enemyComp.attackRange = 1.0f;
            enemyComp.damage = 1.0f;
            enemyComp.health = 1.0f;
            enemyComp.attackCooldown = 1.0f;
            enemyComp.timeSinceLastAttack = 0.0f;

            // Register all components
            registry.AddComponents<
                MeshComponent,
                TransformComponent,
                TextureComponent,
                AIComponent,
                EnemyComponent,
                CollisionComp
            >(
                enemy,
                std::move(meshComp),
                std::move(transform),
                std::move(texture),
                std::move(ai),
                std::move(enemyComp),
                std::move(collider)
            );
        }

    } // namespace GameLib
} // namespace KGR