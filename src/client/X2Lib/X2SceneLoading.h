#pragma once

// The scene loading screen (docs/porting/client-x2lib.md, scene loading). Entering a village or a field builds the
// world at once but leaves its meshes and textures to the device manager's loading thread, so the first frames showed
// the character in an empty world. This screen, the first login's layout (DLG_First_Loading_Front.lua, bar along the
// bottom; the destination's own art in place of its background when there is one), covers the scene from the state
// change until the loading queues are idle, the server confirmed the entry and the scene was drawn a few times behind
// it, then fades out. Its bar follows the background loads.
//
// It is CKTDXApp's overlay stage: moved and drawn after every stage, so it outlives the state changes it covers.
class CX2SceneLoading : public CKTDXStage
{
public:
	CX2SceneLoading();
	virtual ~CX2SceneLoading();

	// Shows the screen (or restarts its wait if it is up) and presents one frame of it at once, before the caller's
	// synchronous loading.
	void Begin();
	bool IsActive() const { return m_eStep != STEP_IDLE; }

	virtual HRESULT OnFrameMove( double fTime, float fElapsedTime ) override;
	virtual HRESULT OnFrameRender() override;

private:
	enum EStep
	{
		STEP_IDLE,
		STEP_LOADING,	// waiting for the scene
		STEP_FADE_OUT,
	};

	void CreateLoadingDialog();
	void SetBackground();
	bool IsStateReady() const;
	bool IsCoveredState() const;
	void SetProgress( float fProgress );
	void Close();

	EStep					m_eStep;
	CKTDGUIDialogType		m_pDialog;
	CKTDXDeviceTexture*		m_pTextureBlack;	// fills the screen around the 4:3 picture

	uint32_t				m_uFinishedAtBegin;	// the device manager's finished loads when the wait began
	uint32_t				m_uLastQueued;		// its queued loads at the last frame, to notice new ones
	float					m_fTime;			// since Begin
	float					m_fReadyTime;		// how long everything has been ready
	int						m_iReadyFrames;		// frames drawn since everything is ready
	float					m_fShownProgress;	// 0..1, what the bar shows (never goes back)
	float					m_fAlpha;
};
