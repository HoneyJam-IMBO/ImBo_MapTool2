#include "stdafx.h"
#include "Space.h"
#include "SpaceContainer.h"
#include "RenderContainerSeller.h"

void CSpace::Begin(CSpaceContainer * pSpaceContainer, UINT size, int lv, XMVECTOR pos){
	//object_id set 
//	m_objectID = object_id::OBJECT_SPACE;
	CGameObject::Begin();
	//size를 알아야 할지도 모르니까 일단 저장
	m_size = size;
	m_pSpaceContainer = pSpaceContainer;

	//자신의 위치를 정해줌 
	//공간의 중간이 아니라 제일 처음/ 좌표가 시작 좌표
	SetPosition(pos);

	//현재 래밸 --
	lv -= 1;

	//0보다 작으면 return 마지막 단계라는 것
	/*
	ex) 처음 lootSpace의 Begin의 lv에 1을 넣었다면
	들어와서 -1 하겟지? 그럼 0 이야 0보다 작지 않아
	그럼 자식을 4개 만들꺼야. 1은 1단계
	0은 공간분할 없음.
	*/
	if (lv < 0) {
		//자식을 가지지 않음 -> leaf space임
		//leaf space는 자신의 index를 알고 있어야 함
		//자신의 index를 계산 후 저장
		m_index = m_pSpaceContainer->SearchSpace(GetPosition());
		//space container에 자신을 등록
		m_pSpaceContainer->AddSpace(m_index, this);

		float fx = static_cast<float>(m_pSpaceContainer->GetOneSpaceSize());
		float fy = static_cast<float>(m_pSpaceContainer->GetSpaceSize());
		float fz = static_cast<float>(m_pSpaceContainer->GetOneSpaceSize());

		BoundingBox::CreateFromPoints(m_OriBoundingBox, XMVectorSet(0.f, -fy, 0.f, 0.f), XMVectorSet(fx, fy, fz, 0.f));
		
		//SetRenderContainer(pSeller);//그림을 그릴 수도 있으니 RenderContainer set
		return;
	}

	//자식 공간 할당
	m_ppChildSpace = new CSpace*[4];
	//aabb
	BoundingBox::CreateFromPoints(m_OriBoundingBox, XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(static_cast<float>(size), static_cast<float>(m_pSpaceContainer->GetSize()), static_cast<float>(size), 0.f));


	int k = 0;
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < 2; ++i) {
			UINT child_size = (size / 2);
			//x먼저 증가, 이후 z증가.
			/* 자식 공간의 순서
			2 3
			0 1 
			*/
			XMVECTOR xmvOffset = XMVectorSet(static_cast<float>(i*child_size), 0.f, static_cast<float>(j*child_size), 0.f);
			m_ppChildSpace[k] = new CSpace();
			m_ppChildSpace[k++]->Begin(pSpaceContainer, child_size, lv, GetPosition() + xmvOffset);
		}
	}
	m_mlpObject.clear();
	m_mlpCollisionObject.clear();
}

bool CSpace::End(){
	for (auto mlp : m_mlpObject) {
		for (auto pObject : mlp.second) {
			pObject->End();
			delete pObject;
		}
		mlp.second.clear();
	}
	m_mlpObject.clear();

	if (m_ppChildSpace) {
		for (int i = 0; i < 4; ++i) {
			m_ppChildSpace[i]->End();
			m_ppChildSpace[i] = nullptr;
		}
	}
	m_ppChildSpace = nullptr;

	m_mlpCollisionObject.clear();

	delete this;

	return true;
}

void CSpace::Animate(float fTimeElapsed){
	

	if (m_ppChildSpace) {
		for (int i = 0; i < 4; ++i) {
			m_ppChildSpace[i]->Animate(fTimeElapsed);
		}
	}
	else {//leaf space만 animate
		//dynamic object만 animate 
		//animate하는데 이전의 space index와 현재 index가 다르다면 
		int nObject = 0;

		vector<CGameObject*> lpDeleteObject;
		for (auto pObject : m_mlpObject[tag::TAG_DYNAMIC_OBJECT]) {
			pObject->Animate(fTimeElapsed);
			int current_index = m_pSpaceContainer->SearchSpace(pObject->GetPosition());
			if (pObject->GetSpaceIndex() != current_index) {//이전 공간 index와 현재 index가 다르다면
				m_pSpaceContainer->AddBlockObjectList(pObject);//block Object list에 등록
				lpDeleteObject.push_back(pObject);
			}
			nObject++;
		}
		if(INPUTMGR->GetDebugMode())
			DEBUGER->AddText(10.0f, 250.0f, static_cast<float>(m_index * 15.f), YT_Color(255, 255, 255), L"space %d object_num : %d", m_index, nObject);


		for (auto pObject : lpDeleteObject) {
			RemoveObject(pObject);//해당 객체 remove
		}

	}

}

void CSpace::PhisicsUpdate(float fTimeElapsed){
	

	for (auto pPlayer : m_mlpCollisionObject[utag::UTAG_PLAYER]) {
		//player control
		if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_CHARACTER) {
			DWORD dwDirection = 0;
			//UINT iMoveState;
			float SPEED = 50.f;

			XMVECTOR xmvLook = pPlayer->GetLook();
			XMFLOAT3 xmf3Look;
			XMStoreFloat3(&xmf3Look, xmvLook);
			XMVECTOR xmvRight = pPlayer->GetRight();
			XMFLOAT3 xmf3Right;
			XMStoreFloat3(&xmf3Right, xmvRight);
			XMVECTOR xmvUp = pPlayer->GetUp();
			XMFLOAT3 xmf3Up;
			XMStoreFloat3(&xmf3Up, xmvUp);

			XMVECTOR xmvShift = XMVectorSet(0.0f, 0.0f, 0.0f, 0.f);
			if (INPUTMGR->OnlyKeyBoardDown(VK_SHIFT_)) SPEED *= 10;
			if (INPUTMGR->OnlyKeyBoardDown(VK_W))      dwDirection |= DIR_FORWARD;
			if (INPUTMGR->OnlyKeyBoardDown(VK_S))      dwDirection |= DIR_BACKWARD;
			if (INPUTMGR->OnlyKeyBoardDown(VK_A))      dwDirection |= DIR_LEFT;
			if (INPUTMGR->OnlyKeyBoardDown(VK_D))      dwDirection |= DIR_RIGHT;
			if (INPUTMGR->OnlyKeyBoardDown(VK_E))      dwDirection |= DIR_UP;
			if (INPUTMGR->OnlyKeyBoardDown(VK_Q))      dwDirection |= DIR_DOWN;
			if (dwDirection)
			{
				if (dwDirection & DIR_FORWARD)      xmvShift += XMLoadFloat3(&xmf3Look);
				if (dwDirection & DIR_BACKWARD)      xmvShift -= XMLoadFloat3(&xmf3Look);
				if (dwDirection & DIR_RIGHT)      xmvShift += XMLoadFloat3(&xmf3Right);
				if (dwDirection & DIR_LEFT)         xmvShift -= XMLoadFloat3(&xmf3Right);
				if (dwDirection & DIR_UP)         xmvShift += XMLoadFloat3(&xmf3Up);
				if (dwDirection & DIR_DOWN)         xmvShift -= XMLoadFloat3(&xmf3Up);

				XMVector3Normalize(xmvShift);

				//pos에 shift*speed*ftimeElapsed 더해줌 = 속도만큼 이동
				pPlayer->Move(xmvShift, SPEED*fTimeElapsed);
				//XMFLOAT3 xmf3Pos;
				//XMStoreFloat3(&xmf3Pos, pPlayer->GetPosition());
				//pPlayer->SetPosition(XMLoadFloat3(&xmf3Pos) + ((xmvShift * SPEED) * fTimeElapsed));
			}

		}
		//player control

		pPlayer->SetUTag(UTAG_PLAYER);
		for (auto pObj : m_mlpCollisionObject[utag::UTAG_COLLISION]) {
			if (pObj->IsCollision(pPlayer)) {
				pPlayer->SetUTag(UTAG_COLLISION);
			}
		}
		for (auto pObj : m_mlpCollisionObject[utag::UTAG_COLLISION_MULTIOBB]) {
			if (pObj->IsCollision(pPlayer)) {
				pPlayer->SetUTag(UTAG_COLLISION);
			}
		}
	}

	for (auto pPlayer : m_mlpCollisionObject[utag::UTAG_BOSS1]) {
		pPlayer->SetUTag(UTAG_BOSS1);
		for (auto pObj : m_mlpCollisionObject[utag::UTAG_BOSS2]) {
			if (pObj->IsCollision(pPlayer)) {
				pPlayer->SetUTag(UTAG_COLLISION);
			}
		}
	}
}

void CSpace::PrepareRender(UINT renderFlag){
	//나는 그리는 space다.
	SetbRender(true);
	this->RegistToDebuger();
	//RegistToContainer();
	//			DEBUGER->RegistToDebugRenderContainer(this);
	for (auto mlp : m_mlpObject) {//모든 객체에 대해서
								  //자신이 속한 rendercontainer에 등록
		for (auto pObject : mlp.second) {
			
				if (renderFlag & TAG_TERRAIN) {
					if (pObject->GetTag() == TAG_TERRAIN) pObject->RegistToContainer();
				}
				if (renderFlag & TAG_STATIC_OBJECT) {
					if (pObject->GetTag() == TAG_STATIC_OBJECT) pObject->RegistToContainer();
				}
				if (renderFlag & TAG_DYNAMIC_OBJECT) {
					if (pObject->GetTag() == TAG_DYNAMIC_OBJECT) pObject->RegistToContainer();
				}
				if (renderFlag & TAG_LIGHT) {
					if (pObject->GetTag() == TAG_LIGHT) pObject->RegistToContainer();
				}
				if (renderFlag & TAG_BIGWATER) {
					if (pObject->GetTag() == TAG_BIGWATER) pObject->RegistToContainer();
				}
		}
	}//end for
}

void CSpace::PrepareRender( CCamera* pCamera, UINT renderFlag){
	
	if (IsVisible(pCamera)) {//여기에 space 프러스텀 컬링
		if (nullptr == m_ppChildSpace) {//내 자식이 없으면 나는 leaf node
			//나는 그리는 space다.
			SetbRender(true);
			this->RegistToDebuger();
			//RegistToContainer();
//			DEBUGER->RegistToDebugRenderContainer(this);
			for (auto mlp : m_mlpObject) {//모든 객체에 대해서
				//자신이 속한 rendercontainer에 등록
				if (mlp.first == TAG_TERRAIN) {
					for (auto pObject : mlp.second) {
						pObject->RegistToContainer();
					}
				}
				if (mlp.first == TAG_STATIC_OBJECT) {
					for (auto pObject : mlp.second) {
						pObject->RegistToContainer();
					}
				}
				if (mlp.first == TAG_DYNAMIC_OBJECT) {
					for (auto pObject : mlp.second) {
						pObject->RegistToContainer();
					}
				}
				if (mlp.first == TAG_TERRAIN) {
					for (auto pObject : mlp.second) {
						pObject->RegistToContainer();
					}
				}
				if (mlp.first == TAG_REFLECTION) {
					for (auto pObject : mlp.second) {
						pObject->RegistToContainer();
					}
				}
				if (mlp.first == TAG_LIGHT) {
					for (auto pObject : mlp.second) {
						pObject->RegistToContainer();
					}
				}
				if (mlp.first == TAG_BIGWATER) {
					for (auto pObject : mlp.second) {
						pObject->RegistToContainer();
					}
				}
			}//end for
		}//end if
		else {//leaf가 아니라면
			for (int i = 0; i < 4; ++i) {
				m_ppChildSpace[i]->PrepareRender(pCamera, renderFlag);//내 자식들 PrePareRender
			}
		}//end else
	}
	//카메라에 보이지 않으면 
	return;
}


void CSpace::SetObejcts(int n, CGameObject** ppObjects) {
	if (!ppObjects) return;

	for (int i = 0; i < n; ++i) {
		//객체에 자신 번호 등록
		ppObjects[i]->SetSpaceIndex(m_index);
		m_mlpObject[ppObjects[i]->GetTag()].emplace_back(ppObjects[i]);
	}
}

void CSpace::AddObject(CGameObject* pObject) {
	if (!pObject) return;
	//객체에 자신 번호 등록
	pObject->SetSpaceIndex(m_index);
	m_mlpCollisionObject[pObject->GetUTag()].emplace_back(pObject);
	m_mlpObject[pObject->GetTag()].emplace_back(pObject);
}
void CSpace::RemoveObject(CGameObject* pObject) {
	if (!pObject) return;
	//if (0 == m_lpObjects.size()) return;

	m_mlpObject[pObject->GetTag()].remove_if([&pObject](CGameObject* pO) {
		return pObject == pO;
	});
	m_mlpCollisionObject[pObject->GetUTag()].remove_if([&pObject](CGameObject* pO) {
		return pObject == pO;
	});
}

void CSpace::RemoveObject(string name){
	for (auto data : m_mlpObject) {
		for (auto pObject : data.second) {
			if (pObject->GetName() == name) {
				m_mlpObject[pObject->GetTag()].remove_if([&pObject](CGameObject* pO) {
					return pObject == pO;
				});
			}
		}
	}
	for (auto data : m_mlpCollisionObject) {
		for (auto pObject : data.second) {
			if (pObject->GetName() == name) {
				m_mlpCollisionObject[pObject->GetUTag()].remove_if([&pObject](CGameObject* pO) {
					return pObject == pO;
				});
			}
		}
	}
}

void CSpace::GetMainBoundingBox(BoundingBox& out) {
	out = m_OriBoundingBox;

	out.Transform(out, GetWorldMtx());
}

CGameObject * CSpace::PickObject(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float& distance){
	float fHitDistance = FLT_MAX;
	distance = fHitDistance;
	float fNearHitDistance = FLT_MAX;
	CGameObject* pObj = nullptr;
	//자신의 모든 객체에 대해서 검사
	for (auto Objects : m_mlpObject) {
		for (auto pObject : Objects.second) {
			if (pObject->CheckPickObject(xmvWorldCameraStartPos, xmvRayDir, fHitDistance)) {//ray와 충돌했다면
				if (fNearHitDistance > fHitDistance) {//이전의 가장 가까운 녀석과 비교
					fNearHitDistance = fHitDistance;
					distance = fHitDistance;//더 가까우면 가장 가까운 객체 변경
					pObj = pObject;
				}
			}
		}
	}
	return pObj;//해당 객체 return
}


CSpace::CSpace() : CGameObject("space", tag::TAG_SPACE){

}

CSpace::~CSpace()
{
}
