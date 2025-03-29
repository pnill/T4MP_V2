#include ""
DMPlayer* T4MP::GetDMPlayer(int index)
{
	T4Engine * TurokEngine = (T4Engine*)0x6B52E4;
	if (TurokEngine->pT4Game)
		if (TurokEngine->pT4Game->pEngineObjects)
			if (TurokEngine->pT4Game->pEngineObjects->pCameraArray[index])
				if (TurokEngine->pT4Game->pEngineObjects->pCameraArray[index]->pActor)
				{
					if (TurokEngine->pT4Game->pEngineObjects->pCameraArray[index]->pActor->pDMPlayer)
						return TurokEngine->pT4Game->pEngineObjects->pCameraArray[index]->pActor->pDMPlayer;
				}
				else
				{
					return (DMPlayer*)TurokEngine->pT4Game->pEngineObjects->pCameraArray[index]->pPlayer;
				}


	return 0;
}