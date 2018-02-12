// BeatBoxersFBXBaker.cpp : Defines the entry point for the console application.
// Depends on FBXSDK v2017.0.1 and DevIL v1.8.0

#include "stdafx.h"
#include "Deform.h"

const int ERR_ARGS = -1;
const int ERR_FBXSDK = -2;
const int ERR_MERGE = -3;
const int ERR_BAKE = -4;

using namespace std;

void PrintNode(FbxNode* pNode);
int process(string inputModelFile, string inputAnimationFile, string outputName);
void NumberVerts(FbxScene* scene);

void usage(char* name)
{
	printf("Usage: %s <model> <animation> [output]\n", name);
}

int main(int argc, char** argv)
{
	string inputModelFile;
	string inputAnimationFile;
	string outputName;
	int retval = 0;
	if (argc <= 2)
	{
		usage(argv[0]);
		retval = ERR_ARGS;
		printf("\nRecieved %i args:\n", argc);
		for (int i = 0; i < argc; i++)
		{
			printf("\t%s\n", argv[i]);
		}
	}
	else if (argc > 2)
	{
		inputModelFile = string(argv[1]);
		inputAnimationFile = string(argv[2]);
		if (argc > 3)
		{
			outputName = string(argv[3]);
		}
		else
		{
			if (inputModelFile.rfind(".fbx") != inputModelFile.npos)
			{
				outputName = inputModelFile.substr(0, inputModelFile.rfind(".fbx"));
			}
			else
			{
				outputName = inputModelFile;
			}
		}
		retval = process(inputModelFile, inputAnimationFile, outputName);
	}
	if (retval < 0)
	{
		printf("Execution ended with error code %i.\n", retval);
	}
	printf("Press enter to exit.");
	getchar();
	return retval;
}

void PrintDetails(FbxScene* scene)
{
	if (scene == nullptr) return;
	FbxNode* lRootNode = scene->GetRootNode();
	if (lRootNode)
	{
		for (int i = 0; i < lRootNode->GetChildCount(); i++)
			PrintNode(lRootNode->GetChild(i));
	}
}

bool LoadScene(FbxManager* lSdkManager, FbxScene*& outScene, string filename)
{
	if (lSdkManager == nullptr) return false;

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	printf("Loading fbx %s...\n", filename.c_str());
	if (!lImporter->Initialize(filename.c_str(), -1, lSdkManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		return false;
	}

	// Create a new scene so that it can be populated by the imported file.
	outScene = FbxScene::Create(lSdkManager, "");

	// Import the contents of the file into the scene.
	lImporter->Import(outScene);

	// The file is imported, so get rid of the importer.
	lImporter->Destroy();
	return true;
}

bool SaveScene(FbxManager* lSdkManager, FbxScene* inScene, string outFilename)
{
	// Create an exporter.
	FbxExporter* lExporter = FbxExporter::Create(lSdkManager, "");

	int pFileFormat = -1;

	//Try to export in ASCII if possible
	int lFormatIndex, lFormatCount =
		lSdkManager->GetIOPluginRegistry()->
		GetWriterFormatCount();

	for (lFormatIndex = 0; lFormatIndex<lFormatCount; lFormatIndex++)
	{
		if (lSdkManager->GetIOPluginRegistry()->
			WriterIsFBX(lFormatIndex))
		{
			FbxString lDesc = lSdkManager->GetIOPluginRegistry()->
				GetWriterFormatDescription(lFormatIndex);
			if (lDesc.Find("ascii") >= 0)
			{
				pFileFormat = lFormatIndex;
				break;
			}
		}
	}

	printf("Saving %s.\n", outFilename.c_str());
	if (lExporter->Initialize(outFilename.c_str(), pFileFormat) == false)
	{
		printf("Call to FbxExporter::Initialize() failed.\n");
		printf("Error returned: %s\n", lExporter->GetStatus().GetErrorString());
		return false;
	}

	// Export the scene.
	lExporter->Export(inScene);

	// Destroy the exporter.
	lExporter->Destroy();
	return true;
}

bool MergeScenes(FbxScene* hostScene, FbxScene* sourceScene)
{
	if (hostScene == nullptr || sourceScene == nullptr) return false;
	bool allgood = true;

	// Move other objects to the reference scene.
	int id = 0;
	if (sourceScene->GetNameOnly().IsEmpty())
	{
		char name[8];
		sprintf_s(name, "UO#%i", id++);
		sourceScene->SetName(name);
	}
	int lNumSceneObjects = sourceScene->GetSrcObjectCount();
	for (int objIndex = 0; objIndex < lNumSceneObjects; objIndex++)
	{
		FbxObject* lObj = sourceScene->GetSrcObject(objIndex);
		if (
			lObj == nullptr ||
			lObj == sourceScene->GetRootNode() ||
			lObj == &sourceScene->GetGlobalSettings()
			)
		{
			// Don't move the root node or the scene's global settings; these
			// objects are created for every scene.
			continue;
		}

		string className = string(lObj->GetClassId().GetName());
		if (className.compare("FbxAnimCurveNode") == 0)
		{
			for (int dpropindex = 0; dpropindex < lObj->GetDstPropertyCount(); dpropindex++)
			{
				FbxProperty dprop = lObj->GetDstProperty(dpropindex);
				FbxObject* dstPropObj = dprop.GetFbxObject();
				string dstPropClass = dstPropObj->GetClassId().GetName();
				if (dstPropClass.compare("FbxNode") != 0)
				{
					printf("ERROR: Unexpected property destination class for FbxNodeAnimCurveNode \"%s\", expected FbxNode.\n", lObj->GetName());
					allgood = false;
				}
				else
				{
					FbxNode* hostNode = nullptr;
					for (int i = 0; i < hostScene->GetSrcObjectCount<FbxNode>(); i++)
					{
						FbxNode* tempNode = (FbxNode*)hostScene->GetSrcObject<FbxNode>(i);
						if (tempNode->GetNameOnly().Compare(dprop.GetFbxObject()->GetNameOnly()) == 0)
						{
							hostNode = tempNode;
							break;
						}
					}
					if (hostNode == nullptr)
					{
						printf("ERROR: Unable to find corresponding FbxNode \"%s\" in provided model.\n", lObj->GetName());
						allgood = false;
					}
					else
					{
						FbxProperty newProp = FbxProperty::CreateFrom(hostNode, dprop);
						lObj->ConnectDstProperty(newProp);
						lObj->DisconnectDstProperty(dprop);
					}
				}
			}
			lObj->ConnectDstObject(hostScene);
		}
		else if (
			className.compare("FbxAnimStack") == 0
			|| className.compare("FbxAnimLayer") == 0
			|| className.compare("FbxAnimCurveNode") == 0
			|| className.compare("FbxAnimCurve") == 0
			)
		{
			// Attach the object to the reference scene.
			lObj->ConnectDstObject(hostScene);
		}
	}

	// Disconnect all scene objects.
	sourceScene->DisconnectAllSrcObject();

	return allgood;
}

/* Tab character ("\t") counter */
int numTabs = 0;

/**
* Print the required number of tabs.
*/
void PrintTabs()
{
	for (int i = 0; i < numTabs; i++)
		printf("\t");
}

/**
* Return a string-based representation based on the attribute type.
*/
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type)
	{
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

/**
* Print an attribute.
*/
void PrintAttribute(FbxNodeAttribute* pAttribute)
{
	if (!pAttribute) return;

	FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
	FbxString attrName = pAttribute->GetName();
	PrintTabs();
	// Note: to retrieve the character array of a FbxString, use its Buffer() method.
	printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}

/**
* Print a node, its attributes, and all its children recursively.
*/
void PrintNode(FbxNode* pNode)
{
	PrintTabs();
	const char* nodeName = pNode->GetName();
	FbxDouble3 translation = pNode->LclTranslation.Get();
	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();

	// Print the contents of the node.
	printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
		nodeName,
		translation[0], translation[1], translation[2],
		rotation[0], rotation[1], rotation[2],
		scaling[0], scaling[1], scaling[2]
	);
	numTabs++;

	// Print the node's attributes.
	for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
		PrintAttribute(pNode->GetNodeAttributeByIndex(i));

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++)
		PrintNode(pNode->GetChild(j));

	numTabs--;
	PrintTabs();
	printf("</node>\n");
}

void NumberVerts(FbxScene* pScene)
{
	if (pScene == nullptr) return;

	FbxMesh* pMesh = nullptr;

	for (int i = 0; i < pScene->GetRootNode()->GetChildCount(); i++)
	{
		FbxNode* pNode = pScene->GetRootNode()->GetChild(i);
		if (pNode != nullptr)
		{
			pMesh = pNode->GetMesh();
			if (pMesh != nullptr) break;
		}
	}

	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);

	FbxLayer* pLayer = pMesh->GetLayer(0);
	FbxLayerElementVertexColor* pVertColor = pLayer->GetVertexColors();
	if (pVertColor == nullptr)
	{
		pLayer->CreateLayerElementOfType(FbxLayerElement::eVertexColor);
		pVertColor = pLayer->GetVertexColors();
	}
	pVertColor->SetMappingMode(FbxLayerElement::EMappingMode::eByControlPoint);
	if (pVertColor->GetDirectArray().GetCount() > 0)
	{
		printf("ERROR: Preexisting data in vertex color.\n");
	}
	else
	{
		FbxColor VertColor;
		for (int i = 0; i < pMesh->GetControlPointsCount(); i++)
		{
			VertColor.mRed = FbxMin<double>(i % 8 / 7., 1.);
			VertColor.mGreen = FbxMin<double>((i >> 3) % 8 / 7., 1.);
			VertColor.mBlue = FbxMin<double>((i >> 6) % 8 / 7., 1.);
			VertColor.mAlpha = FbxMin<double>((i >> 9) % 8 / 7., 1.);
			pVertColor->GetDirectArray().Add(VertColor);
		}
		printf("Added vert colors for %i vertices.\n", pMesh->GetControlPointsCount());
	}
}

bool bake(FbxScene* scene, string outTexturefile)
{
	if (scene == nullptr)
	{
		printf("ERROR: bake recieved a null scene.\n");
		return false;
	}

	FbxNode* root = scene->GetRootNode();
	if (root == nullptr)
	{
		printf("ERROR: unable to get scene root for bake.\n");
		return false;
	}

	FbxMesh* mesh = scene->GetSrcObject<FbxMesh>();
	if (mesh == nullptr)
	{
		printf("ERROR: unable to get mesh node for bake.\n");
		return false;
	}

	FbxArray<FbxString*> mAnimStackNameArray;
	scene->FillAnimStackNameArray(mAnimStackNameArray);
	if (mAnimStackNameArray.GetCount() <= 0)
	{
		printf("ERROR: scene contains no animation.\n");
		return false;
	}

	FbxAnimStack *lCurrentAnimationStack = scene->FindMember<FbxAnimStack>(mAnimStackNameArray[0]->Buffer());
	if (lCurrentAnimationStack == nullptr)
	{
		printf("ERROR: unable to find animation stack for bake.\n");
		return false;
	}

	FbxPose* pose = scene->GetSrcObject<FbxPose>();
	if (pose == nullptr)
	{
		printf("ERROR: unable to get bind pose for bake.\n");
		return false;
	}

	FbxTime mStart, mStop, mFrameTime, mCurrentTime;
	mFrameTime.SetTime(0, 0, 0, 1, 0, scene->GetGlobalSettings().GetTimeMode());

	// we assume that the first animation layer connected to the animation stack is the base layer
	// (this is the assumption made in the FBXSDK)
	FbxAnimLayer *mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
	scene->SetCurrentAnimationStack(lCurrentAnimationStack);

	FbxTakeInfo* lCurrentTakeInfo = scene->GetTakeInfo(*(mAnimStackNameArray[0]));
	if (lCurrentTakeInfo != nullptr)
	{
		mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
		mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
	}
	else
	{
		mStart = lCurrentAnimationStack->LocalStart.Get();
		mStop = lCurrentAnimationStack->LocalStop.Get();
	}

	FbxTimeSpan mSpan = FbxTimeSpan(mStart, mStop);
	int numFrames = (int)mSpan.GetDuration().GetFrameCount(scene->GetGlobalSettings().GetTimeMode());

	printf("Animation goes from %s to %s in %i frames.\n"
		,mStart.GetTimeString().Buffer()
		,mStop.GetTimeString().Buffer()
		,numFrames
	);

	int numControlPoints = mesh->GetControlPointsCount();
	FbxVector4 *controlPoints = mesh->GetControlPoints();
	FbxVector4 *deformPoints = new FbxVector4[numControlPoints];
	FbxVector4* diff = new FbxVector4[numControlPoints * numFrames];

	printf("GetPolygonVertexCount %i\n", mesh->GetPolygonVertexCount());

	FbxArray<FbxVector4> normals;
	if (!mesh->GetPolygonVertexNormals(normals))
	{
		printf("Unable to get polygon vertex normals.\n");
	}
	else
	{
		printf("%i polygon vertex normals.\n", normals.GetCount());
	}
	FbxLayerElementNormal *pNormals;
	if (!mesh->GetNormals((FbxLayerElementArrayTemplate<FbxVector4>**)&pNormals))
	{
		printf("Unable to get normals.\n");
	}
	else
	{
		if (pNormals == nullptr)
		{
			printf("Says it passed, but still nullptr.\n");
		}
		else
		{
			switch (pNormals->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				printf("Says its direct.\n");
				break;
			case FbxLayerElement::eIndex:
			case FbxLayerElement::eIndexToDirect:
				printf("%i indices.\n", pNormals->mIndexArray->GetCount());
				break;
			}
		}
	}

	int largeframe = -1;
	double largest = 0;

	int frame = 0;
	for (mCurrentTime = mStart; mCurrentTime < mStop; mCurrentTime += mFrameTime)
	{
		Deform(mesh, mCurrentTime, mCurrentAnimLayer, mesh->GetNode()->EvaluateLocalTransform(), nullptr, deformPoints);
		for (int i = 0; i < numControlPoints; i++)
		{
			diff[frame * numControlPoints + i] = deformPoints[i] - controlPoints[i];
			for (int j = 0; j < 3; j++)
			{
				if (FbxAbs(diff[frame * numControlPoints + i].mData[j]) > largest)
				{
					largest = FbxAbs(diff[frame * numControlPoints + i].mData[j]);
					largeframe = frame;
				}
			}
		}
		/*
		{
			int i = 0;
			printf("f%s, v%i, m{%f, %f, %f, %f}, a{%f, %f, %f, %f}, d{%f, %f, %f, %f}\n"
				,mCurrentTime.GetTimeString()
				,i
				,controlPoints[i].mData[0]
				,controlPoints[i].mData[1]
				,controlPoints[i].mData[2]
				,controlPoints[i].mData[3]
				,deformPoints[i].mData[0]
				,deformPoints[i].mData[1]
				,deformPoints[i].mData[2]
				,deformPoints[i].mData[3]
				,diff[frame * numControlPoints].mData[0]
				,diff[frame * numControlPoints].mData[1]
				,diff[frame * numControlPoints].mData[2]
				,diff[frame * numControlPoints].mData[3]
			);
		}
		*/
		frame++;
	}
	printf("\n");

	int iWidth = 1;
	int iHeight = 1;

	for (; iWidth < numControlPoints; iWidth *= 2);
	for (; iHeight < numFrames; iHeight *= 2);

	printf("Image dimensions: %ix%i, scale by %f\n", iWidth, iHeight, largest);

	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION)
	{
		printf("ERROR: DevIL version mismatch, unable to save image output.\n");
		delete[] diff;
		delete[] deformPoints;
		return false;
	}

	ilInit();
	iluInit();
	ilEnable(IL_FILE_OVERWRITE);

	ILuint ImageName;
	ImageName = ilGenImage();
	ilBindImage(ImageName);

	float *data = new float[iWidth * iHeight * 3];

	for (int vert = 0; vert < iWidth; vert++)
	{
		for (int frame = 0; frame < iHeight; frame++)
		{
			int index = (vert + iWidth * (iHeight - frame - 1)) * 3;
			if (vert < numControlPoints && frame < numFrames)
			{
				FbxVector4 delta;
				delta = diff[frame * numControlPoints + vert];
				for (int i = 0; i < 3; i++)
				{
					data[index + i] = FbxClamp<FbxFloat>((float)(delta.mData[i] / largest / 2. + 0.5), 0.f, 1.f);
				}
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					data[index + i] = 0.5f;
				}
			}
		}
	}

	if (!ilTexImage(iWidth, iHeight, 0, 3, IL_RGB, IL_FLOAT, data))
	{
		printf("ERROR: DevIL unable to set image data: %s.\n", (const char *)iluErrorString(ilGetError()));
		delete[] data;
		delete[] diff;
		delete[] deformPoints;
		return false;
	}

	//DevIL wants a const wchar_t pointer, but it still uses it as if it was a const char pointer.
	if (!ilSave(IL_TGA, (const wchar_t *) outTexturefile.c_str()))
	{
		//Again, says its a wchar_t but is actually using char. No idea what is going on here.
		printf("ERROR: DevIL was unable to save image file: %s.\n", (const char *)iluErrorString(ilGetError()));
		delete[] data;
		delete[] diff;
		delete[] deformPoints;
		return false;
	}

	delete[] data;
	delete[] diff;
	delete[] deformPoints;
	return true;
}

int process(string inputModelFile, string inputAnimationFile, string outputName)
{
	// Initialize the SDK manager. This object handles memory management.
	FbxManager* lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	FbxScene *modelScene, *animationScene;
	if (
		!LoadScene(lSdkManager, modelScene, inputModelFile) ||
		!LoadScene(lSdkManager, animationScene, inputAnimationFile)
		)
	{
		return ERR_FBXSDK;
	}


	printf("Model %s details:\n", inputModelFile.c_str());
	PrintDetails(modelScene);
	printf("Animation %s details:\n", inputAnimationFile.c_str());
	PrintDetails(animationScene);

	NumberVerts(modelScene);

	string outFile = outputName;
	outFile.append("_indexed.fbx");

	if (!SaveScene(lSdkManager, modelScene, outFile))
	{
		return ERR_FBXSDK;
	}

	printf("Merging data.\n");
	if (!MergeScenes(modelScene, animationScene))
	{
		printf("Error occured during merge.\n");
		return ERR_MERGE;
	}

	outFile = outputName;
	outFile.append("_animation.tga");
	printf("Baking animation to image.\n");
	if (!bake(modelScene, outFile))
	{
		printf("Error occured during bake.\n");
		return ERR_BAKE;
	}

	// Destroy the SDK manager and all the other objects it was handling.
	lSdkManager->Destroy();

	return 0;
}