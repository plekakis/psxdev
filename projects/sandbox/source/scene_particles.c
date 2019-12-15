#include <base/core/core.h>

MATRIX g_world2Camera;

SVECTOR g_cameraRotation = { 0, 0, 0 };
VECTOR g_cameraPosition = { 0, 0, -ONE / 4};

typedef struct
{
	VECTOR  m_startPosition;
	VECTOR  m_startVelocity;
	VECTOR  m_position;
	uint16	m_age;
	uint16  m_startLifetime;
	uint16  m_slot;
	int16   m_rotation;
	CVECTOR m_startColor;
	CVECTOR m_endColor;
	uint8	m_width;
	uint8   m_height;
	float   m_time;
}PARTICLE;

// Particles are stored in a contiguous block of memory arranged as such:
// [a][a][a][d][d][d][d]
//
// Where a: alive particle
//       d: dead particle
//
// Alive particles are always stored in the front, followed by dead ones.
// This makes it more efficient to process for rendering and uses less memory.
//

#define PARTICLE_COUNT (100)
PARTICLE*	g_particles;
PARTICLE**	g_aliveParticles;

uint16 g_aliveParticleCount;
#define PARTICLE_NULL_SLOT (0xffff)

#define EMITTER_WIDTH (128)
#define EMITTER_HEIGHT (64)
#define EMITTER_DEPTH (128)

static float s = 0.0f;

///////////////////////////////////////////////////
void InitParticle(PARTICLE* io_particle)
{
	VERIFY_ASSERT(io_particle, "null particle");
	
	setVector(&io_particle->m_position, 0, 0, 0);
	setVector(&io_particle->m_startPosition, RAND_RANGE_I32(0, EMITTER_WIDTH) - EMITTER_WIDTH / 2, RAND_RANGE_I32(0, EMITTER_HEIGHT) - EMITTER_HEIGHT / 2, RAND_RANGE_I32(0, EMITTER_DEPTH) - EMITTER_DEPTH / 2);
	setVector(&io_particle->m_startVelocity, RAND_RANGE_I32(-14, 15), RAND_RANGE_I32(40, 70), RAND_RANGE_I32(-10, 12));
	setColor(&io_particle->m_startColor, 255, 128, 0);
	setColor(&io_particle->m_endColor, 64, 32, 0);

	io_particle->m_startLifetime = F32toFP(RAND_RANGE_F32(8.0f, 13.0f));
	io_particle->m_age = 0;
	io_particle->m_width = RAND_RANGE_I32(4, 7);
	io_particle->m_height = io_particle->m_width;
	io_particle->m_slot = PARTICLE_NULL_SLOT;
	io_particle->m_time = 0.0f;
}

///////////////////////////////////////////////////
void InitParticles()
{
	uint16 i = 0;

	g_aliveParticleCount = 0;
	g_particles = (PARTICLE*)Core_Malloc(sizeof(PARTICLE) * PARTICLE_COUNT);
	g_aliveParticles = (PARTICLE**)Core_Malloc(sizeof(PARTICLE*) * PARTICLE_COUNT);

	for (i = 0; i < PARTICLE_COUNT; ++i)
	{
		InitParticle(&g_particles[i]);
	}
}

///////////////////////////////////////////////////
bool IsParticleAlive(PARTICLE* i_particle)
{
	VERIFY_ASSERT(i_particle, "null particle");
	return (i_particle->m_age <= i_particle->m_startLifetime) ? TRUE : FALSE;
}

///////////////////////////////////////////////////
int16 GetParticleRotation(VECTOR* v, VECTOR* origin)
{
	return 0;
}

///////////////////////////////////////////////////
void UpdateParticles()
{
	uint16 i = 0;

	for (i = 0; i < PARTICLE_COUNT; ++i)
	{
		PARTICLE* particle = &g_particles[i];

		// Particle is alive, let's update it:
		if (IsParticleAlive(particle))
		{
			VECTOR p;
			VECTOR v, a, va;
			fixed4_12 t, t2;

			t = F32toFP(particle->m_time);
			t2 = F32toFP((particle->m_time * particle->m_time)) >> 1;

			// Initial state
			copyVector(&p, &particle->m_startPosition);
			copyVector(&v, &particle->m_startVelocity);
			setVector(&a, 0, -9, 0);

			mulVectorF(&v, t);
			mulVectorF(&a, t2);

			// Update
			copyVector(&particle->m_position, &p);	// set current position
			addVector(&particle->m_position, &v);	// add current velocity
			addVector(&particle->m_position, &a);	// add current acceleration
		
			copyVector(&va, &v);
			addVector(&va, &a);

			particle->m_age = t;
			//particle->m_height = particle->m_width + MIN(16, vectorLengthSq(&va));
			particle->m_rotation = 0;// GetParticleRotation(&va, &particle->m_startPosition);
			particle->m_time += 0.1f;

			// First time seeing this particle. Bring it forward.
			if (particle->m_slot == PARTICLE_NULL_SLOT)
			{
				particle->m_slot = g_aliveParticleCount;
				g_aliveParticles[particle->m_slot] = particle;
				++g_aliveParticleCount;
			}
		}
		// Particle is dead, recycle it.
		else
		{
			// First time we're recycling this particle since its creation. Push it to the back and re-initialise it.
			if (particle->m_slot != PARTICLE_NULL_SLOT)
			{				
				InitParticle(particle);
				g_aliveParticles[--g_aliveParticleCount] = particle;
			}
		}
	}
}

///////////////////////////////////////////////////
void RenderParticles()
{
	uint16 i = 0;

	// We have alive particles to render?
	if (g_aliveParticleCount > 0)
	{
		// Allocate a large enough buffer on the core scratch allocator.
		POINT_SPRITE* buffer = (POINT_SPRITE*)Core_AllocScratch(CORE_SCRATCHALLOC, sizeof(POINT_SPRITE) * g_aliveParticleCount, 4);
		
		// Process alive particles:
		for (i = 0; i < g_aliveParticleCount; ++i)
		{
			// Copy to renderable structure.
			PARTICLE* particle = g_aliveParticles[i];

			// Color interpolation.
			SetFarColor(particle->m_endColor.r, particle->m_endColor.g, particle->m_endColor.b);
			DpqColor(&particle->m_startColor, DivFP(particle->m_age, particle->m_startLifetime), &buffer[i].c);
			
			// Copy position.
			copyVector(&buffer[i].p, &particle[i].m_position);

			// Copy rotation.
			buffer[i].r = particle[i].m_rotation;

			// Copy dimensions.
			buffer[i].width = particle[i].m_width;
			buffer[i].height = particle[i].m_height;
		}

		// Draw
		Gfx_AddPointSprites(PRIM_TYPE_POLY_F4, buffer, g_aliveParticleCount);
	}
}

///////////////////////////////////////////////////
void start()
{
	InitParticles();
}

///////////////////////////////////////////////////
void update()
{
	UpdateParticles();
}

///////////////////////////////////////////////////
void render()
{
	Gfx_SetClearColor(0, 0, 0);
	Gfx_SetBackColor(16, 16, 16);

	Gfx_SetLightColor(0, 255, 255, 255);
	Gfx_SetLightVector(0, 0, ONE, -ONE);

	Gfx_BeginSubmission(OT_LAYER_BG);

	// Camera transform
	{
		RotMatrix(&g_cameraRotation, &g_world2Camera);
		TransMatrix(&g_world2Camera, &g_cameraPosition);

		Gfx_SetCameraMatrix(&g_world2Camera);
	}

	RenderParticles();

	Gfx_EndSubmission();
}