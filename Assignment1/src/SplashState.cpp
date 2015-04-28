#include "SplashState.h"
#include "TestState.h"
#include <gl_core_4_4.h>
void SplashState::Init(GLFWwindow* _window, GameStateManager* _gameStateManager) {
	m_window = _window;
	m_timeLeft = 5.0f;
	m_gameStateManager = _gameStateManager;
}
void SplashState::Update(double _dt) {
	m_timeLeft -= _dt;
	if (m_timeLeft <= 0.0f)	{
		if (!m_gameStateManager->Push("Test")) {
			m_gameStateManager->RegisterState("Test", new TestState(m_window, m_gameStateManager));
			m_gameStateManager->Push("Test");
		}		
		m_gameStateManager->Pop();
	}
}
void SplashState::Draw()
{
	glClearColor((float)m_timeLeft / 10.0f, 0.0f, (float)m_timeLeft / 5.0f, 1.0f);
}

