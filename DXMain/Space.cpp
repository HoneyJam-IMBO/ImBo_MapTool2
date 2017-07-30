#include "stdafx.h"
#include "Space.h"
#include "SpaceContainer.h"
#include "RenderContainerSeller.h"

void CSpace::Begin(CSpaceContainer * pSpaceContainer, UINT size, int lv, XMVECTOR pos){
	//object_id set 
//	m_objectID = object_id::OBJECT_SPACE;
	CGameObject::Begin();
	//size�� �˾ƾ� ������ �𸣴ϱ� �ϴ� ����
	m_size = size;
	m_pSpaceContainer = pSpaceContainer;

	//�ڽ��� ��ġ�� ������ 
	//������ �߰��� �ƴ϶� ���� ó��/ ��ǥ�� ���� ��ǥ
	SetPosition(pos);

	//���� ���� --
	lv -= 1;

	//0���� ������ return ������ �ܰ��� ��
	/*
	ex) ó�� lootSpace�� Begin�� lv�� 1�� �־��ٸ�
	���ͼ� -1 �ϰ���? �׷� 0 �̾� 0���� ���� �ʾ�
	�׷� �ڽ��� 4�� ���鲨��. 1�� 1�ܰ�
	0�� �������� ����.
	*/
	if (lv < 0) {
		//�ڽ��� ������ ���� -> leaf space��
		//leaf space�� �ڽ��� index�� �˰� �־�� ��
		//�ڽ��� index�� ��� �� ����
		m_index = m_pSpaceContainer->SearchSpace(GetPosition());
		//space container�� �ڽ��� ���
		m_pSpaceContainer->AddSpace(m_index, this);

		float fx = static_cast<float>(m_pSpaceContainer->GetOneSpaceSize());
		float fy = static_cast<float>(m_pSpaceContainer->GetSpaceSize());
		float fz = static_cast<float>(m_pSpaceContainer->GetOneSpaceSize());

		BoundingBox::CreateFromPoints(m_OriBoundingBox, XMVectorSet(0.f, -fy, 0.f, 0.f), XMVectorSet(fx, fy, fz, 0.f));
		
		//SetRenderContainer(pSeller);//�׸��� �׸� ���� ������ RenderContainer set
		return;
	}

	//�ڽ� ���� �Ҵ�
	m_ppChildSpace = new CSpace*[4];
	//aabb
	BoundingBox::CreateFromPoints(m_OriBoundingBox, XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(static_cast<float>(size), static_cast<float>(m_pSpaceContainer->GetSize()), static_cast<float>(size), 0.f));


	int k = 0;
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < 2; ++i) {
			UINT child_size = (size / 2);
			//x���� ����, ���� z����.
			/* �ڽ� ������ ����
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
	else {//leaf space�� animate
		//dynamic object�� animate 
		//animate�ϴµ� ������ space index�� ���� index�� �ٸ��ٸ� 
		int nObject = 0;

		vector<CGameObject*> lpDeleteObject;
		for (auto pObject : m_mlpObject[tag::TAG_DYNAMIC_OBJECT]) {
			pObject->Animate(fTimeElapsed);
			int current_index = m_pSpaceContainer->SearchSpace(pObject->GetPosition());
			if (pObject->GetSpaceIndex() != current_index) {//���� ���� index�� ���� index�� �ٸ��ٸ�
				m_pSpaceContainer->AddBlockObjectList(pObject);//block Object list�� ���
				lpDeleteObject.push_back(pObject);
			}
			nObject++;
		}
		if(INPUTMGR->GetDebugMode())
			DEBUGER->AddText(10.0f, 250.0f, static_cast<float>(m_index * 15.f), YT_Color(255, 255, 255), L"space %d object_num : %d", m_index, nObject);


		for (auto pObject : lpDeleteObject) {
			RemoveObject(pObject);//�ش� ��ü remove
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

				//pos�� shift*speed*ftimeElapsed ������ = �ӵ���ŭ �̵�
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
	//���� �׸��� space��.
	SetbRender(true);
	this->RegistToDebuger();
	//RegistToContainer();
	//			DEBUGER->RegistToDebugRenderContainer(this);
	for (auto mlp : m_mlpObject) {//��� ��ü�� ���ؼ�
								  //�ڽ��� ���� rendercontainer�� ���
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
	
	if (IsVisible(pCamera)) {//���⿡ space �������� �ø�
		if (nullptr == m_ppChildSpace) {//�� �ڽ��� ������ ���� leaf node
			//���� �׸��� space��.
			SetbRender(true);
			this->RegistToDebuger();
			//RegistToContainer();
//			DEBUGER->RegistToDebugRenderContainer(this);
			for (auto mlp : m_mlpObject) {//��� ��ü�� ���ؼ�
				//�ڽ��� ���� rendercontainer�� ���
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
		else {//leaf�� �ƴ϶��
			for (int i = 0; i < 4; ++i) {
				m_ppChildSpace[i]->PrepareRender(pCamera, renderFlag);//�� �ڽĵ� PrePareRender
			}
		}//end else
	}
	//ī�޶� ������ ������ 
	return;
}


void CSpace::SetObejcts(int n, CGameObject** ppObjects) {
	if (!ppObjects) return;

	for (int i = 0; i < n; ++i) {
		//��ü�� �ڽ� ��ȣ ���
		ppObjects[i]->SetSpaceIndex(m_index);
		m_mlpObject[ppObjects[i]->GetTag()].emplace_back(ppObjects[i]);
	}
}

void CSpace::AddObject(CGameObject* pObject) {
	if (!pObject) return;
	//��ü�� �ڽ� ��ȣ ���
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
	//�ڽ��� ��� ��ü�� ���ؼ� �˻�
	for (auto Objects : m_mlpObject) {
		for (auto pObject : Objects.second) {
			if (pObject->CheckPickObject(xmvWorldCameraStartPos, xmvRayDir, fHitDistance)) {//ray�� �浹�ߴٸ�
				if (fNearHitDistance > fHitDistance) {//������ ���� ����� �༮�� ��
					fNearHitDistance = fHitDistance;
					distance = fHitDistance;//�� ������ ���� ����� ��ü ����
					pObj = pObject;
				}
			}
		}
	}
	return pObj;//�ش� ��ü return
}


CSpace::CSpace() : CGameObject("space", tag::TAG_SPACE){

}

CSpace::~CSpace()
{
}
