#ifndef GUARD_LSL_RUNTIME_LIBRARY_FUNCTIONS_HH_INCLUDED
#define GUARD_LSL_RUNTIME_LIBRARY_FUNCTIONS_HH_INCLUDED

#include <lsl/runtime/types.hh>
#include <lsl/runtime/world/script_fwd.hh>

namespace lsl {
namespace runtime {
namespace lib {

using script::ScriptRef;

// Math
Float llSin(ScriptRef, Float);
Float llCos(ScriptRef, Float);
Float llTan(ScriptRef, Float);
Float llAtan2(ScriptRef, Float);
Float llSqrt(ScriptRef, Float);
Float llPow(ScriptRef, Float, Float);
Float   llLog(ScriptRef, Float);
Integer llAbs(ScriptRef, Integer);
Float llFabs(ScriptRef, Float);
Integer llFloor(ScriptRef, Float);
Integer llCeil(ScriptRef, Float);
Integer llRound(ScriptRef, Float);
Float llAcos(ScriptRef, Float);
Float llAsin(ScriptRef, Float);
Rotation llAxes2Rot(ScriptRef, Vector a, Vector b, Vector c);

Float llVecMag(ScriptRef, Vector);
Vector llVecNorm(ScriptRef, Vector);
Float llVecDist(ScriptRef, Vector, Vector);

Vector llRot2Euler(ScriptRef, Rotation);
Rotation llEuler2Rot(ScriptRef, Vector);
Vector llRot2Fwd(ScriptRef, Rotation);
Vector llRot2Left(ScriptRef, Rotation);
Vector llRot2Up(ScriptRef, Rotation);

Rotation llRotBetween(ScriptRef, Vector, Vector);


// addFunction(10.f, 0.f, dummy_func, "llAxisAngle2Rot", "q", "vf");
// addFunction(10.f, 0.f, dummy_func, "llRot2Axis", "v", "q");
// addFunction(10.f, 0.f, dummy_func, "llRot2Angle", "f", "q");
// addFunction(10.f, 0.f, dummy_func, "llAngleBetween", "f", "qq");
//addFunction(10.f, 0.f, dummy_func, "llLog10", "f", "f");
//addFunction(10.f, 0.f, dummy_func, "llLog", "f", "f");


// Misc
Float llFrand(ScriptRef, Float);

// URL
String llEscapeURL(ScriptRef, String);
String llUnescapeURL(ScriptRef, String);


// String
String llGetSubString(ScriptRef, String, Integer, Integer);
String llDeleteSubString(ScriptRef, String, Integer, Integer);
String llInsertString(ScriptRef, String, Integer, String);
String llToUpper(ScriptRef, String);
String llToLower(ScriptRef, String);
Integer llSubStringIndex(ScriptRef, String, String);
Integer llStringLength(ScriptRef, String);

String llIntegerToBase64(ScriptRef, Integer);
Integer llBase64ToInteger(ScriptRef, String);

String llStringToBase64(ScriptRef, String);
String llBase64ToString(ScriptRef, String);

String llMD5String(ScriptRef, String, Integer);
String llSHA1String(ScriptRef, String, Integer);

String llXorBase64Strings(ScriptRef, String, String);
String llXorBase64StringsCorrect(ScriptRef, String, String);
String llXorBase64(ScriptRef s, String a, String b);

// List
List     llListSort(ScriptRef, List, Integer, Integer);
Integer  llGetListLength(ScriptRef, List);
Integer  llList2Integer(ScriptRef, List, Integer);
Float    llList2Float(ScriptRef, List, Integer);
String   llList2String(ScriptRef, List, Integer);
Key      llList2Key(ScriptRef, List, Integer);
Vector   llList2Vector(ScriptRef, List, Integer);
Rotation llList2Rot(ScriptRef, List, Integer);
List     llList2List(ScriptRef, List, Integer, Integer);
List     llDeleteSubList(ScriptRef, List, Integer, Integer);
List     llListReplaceList(ScriptRef, List, List, Integer, Integer);
Integer  llGetListEntryType(ScriptRef, List, Integer);
String   llList2CSV(ScriptRef, List);
List     llCSV2List(ScriptRef, String);
List     llListRandomize(ScriptRef, List l, Integer stride);
List     llList2ListStrided(ScriptRef, List, Integer, Integer, Integer);
List     llListInsertList(ScriptRef, List, List, Integer);
Integer  llFindList(ScriptRef, List, List);
List     llParseString2List(ScriptRef, String, List, List);
List     llParseStringKeepNulls(ScriptRef, String, List, List);
String   llDumpList2String(ScriptRef, List, String);
Integer     llListFindList(ScriptRef, List, List);


}}}

#if 0
addFunction(10.f, 0.f, dummy_func, "llWhisper", NULL, "is");
addFunction(10.f, 0.f, dummy_func, "llSay", NULL, "is");
addFunction(10.f, 0.f, dummy_func, "llShout", NULL, "is");
addFunction(10.f, 0.f, dummy_func, "llListen", "i", "isks");
addFunction(10.f, 0.f, dummy_func, "llListenControl", NULL, "ii");
addFunction(10.f, 0.f, dummy_func, "llListenRemove", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llSensor", NULL, "skiff");
addFunction(10.f, 0.f, dummy_func, "llSensorRepeat", NULL, "skifff");
addFunction(10.f, 0.f, dummy_func, "llSensorRemove", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llDetectedName", "s", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedKey", "k", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedOwner", "k", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedType", "i", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedPos", "v", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedVel", "v", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedGrab", "v", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedRot", "q", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedGroup", "i", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedLinkNumber", "i", "i");
addFunction(0.f, 0.f, dummy_func, "llDie", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llGround", "f", "v");
addFunction(10.f, 0.f, dummy_func, "llCloud", "f", "v");
addFunction(10.f, 0.f, dummy_func, "llWind", "v", "v");
addFunction(10.f, 0.f, dummy_func, "llSetStatus", NULL, "ii");
addFunction(10.f, 0.f, dummy_func, "llGetStatus", "i", "i");
addFunction(10.f, 0.f, dummy_func, "llSetScale", NULL, "v");
addFunction(10.f, 0.f, dummy_func, "llGetScale", "v", NULL);
addFunction(10.f, 0.f, dummy_func, "llSetColor", NULL, "vi");
addFunction(10.f, 0.f, dummy_func, "llGetAlpha", "f", "i");
addFunction(10.f, 0.f, dummy_func, "llSetAlpha", NULL, "fi");
addFunction(10.f, 0.f, dummy_func, "llGetColor", "v", "i");
addFunction(10.f, 0.2f, dummy_func, "llSetTexture", NULL, "si");
addFunction(10.f, 0.2f, dummy_func, "llScaleTexture", NULL, "ffi");
addFunction(10.f, 0.2f, dummy_func, "llOffsetTexture", NULL, "ffi");
addFunction(10.f, 0.2f, dummy_func, "llRotateTexture", NULL, "fi");
addFunction(10.f, 0.f, dummy_func, "llGetTexture", "s", "i");
addFunction(10.f, 0.2f, dummy_func, "llSetPos", NULL, "v");
addFunction(10.f, 0.f, dummy_func, "llGetPos", "v", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetLocalPos", "v", NULL);
addFunction(10.f, 0.2f, dummy_func, "llSetRot", NULL, "q");
addFunction(10.f, 0.f, dummy_func, "llGetRot", "q", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetLocalRot", "q", NULL);
addFunction(10.f, 0.f, dummy_func, "llSetForce", NULL, "vi");
addFunction(10.f, 0.f, dummy_func, "llGetForce", "v", NULL);
addFunction(10.f, 0.f, dummy_func, "llTarget", "i", "vf");
addFunction(10.f, 0.f, dummy_func, "llTargetRemove", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llRotTarget", "i", "qf");
addFunction(10.f, 0.f, dummy_func, "llRotTargetRemove", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llMoveToTarget", NULL, "vf");
addFunction(10.f, 0.f, dummy_func, "llStopMoveToTarget", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llApplyImpulse", NULL, "vi");
addFunction(10.f, 0.f, dummy_func, "llApplyRotationalImpulse", NULL, "vi");
addFunction(10.f, 0.f, dummy_func, "llSetTorque", NULL, "vi");
addFunction(10.f, 0.f, dummy_func, "llGetTorque", "v", NULL);
addFunction(10.f, 0.f, dummy_func, "llSetForceAndTorque", NULL, "vvi");
addFunction(10.f, 0.f, dummy_func, "llGetVel", "v", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetAccel", "v", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetOmega", "v", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetTimeOfDay", "f", "");
addFunction(10.f, 0.f, dummy_func, "llGetWallclock", "f", "");
addFunction(10.f, 0.f, dummy_func, "llGetTime", "f", NULL);
addFunction(10.f, 0.f, dummy_func, "llResetTime", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llGetAndResetTime", "f", NULL);
addFunction(10.f, 0.f, dummy_func, "llSound", NULL, "sfii");
addFunction(10.f, 0.f, dummy_func, "llPlaySound", NULL, "sf");
addFunction(10.f, 0.f, dummy_func, "llLoopSound", NULL, "sf");
addFunction(10.f, 0.f, dummy_func, "llLoopSoundMaster", NULL, "sf");
addFunction(10.f, 0.f, dummy_func, "llLoopSoundSlave", NULL, "sf");
addFunction(10.f, 0.f, dummy_func, "llPlaySoundSlave", NULL, "sf");
addFunction(10.f, 0.f, dummy_func, "llTriggerSound", NULL, "sf");
addFunction(10.f, 0.f, dummy_func, "llStopSound", NULL, "");
addFunction(10.f, 1.f, dummy_func, "llPreloadSound", NULL, "s");

addFunction(10.f, 0.f, dummy_func, "llGiveMoney", "i", "ki");
addFunction(10.f, 0.1f, dummy_func, "llMakeExplosion", NULL, "iffffsv");
addFunction(10.f, 0.1f, dummy_func, "llMakeFountain", NULL, "iffffisvf");
addFunction(10.f, 0.1f, dummy_func, "llMakeSmoke", NULL, "iffffsv");
addFunction(10.f, 0.1f, dummy_func, "llMakeFire", NULL, "iffffsv");
addFunction(200.f, 0.1f, dummy_func, "llRezObject", NULL, "svvqi");
addFunction(10.f, 0.f, dummy_func, "llLookAt", NULL, "vff");
addFunction(10.f, 0.f, dummy_func, "llStopLookAt", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llSetTimerEvent", NULL, "f");
addFunction(0.f, 0.f, dummy_func, "llSleep", NULL, "f");
addFunction(10.f, 0.f, dummy_func, "llGetMass", "f", NULL);
addFunction(10.f, 0.f, dummy_func, "llCollisionFilter", NULL, "ski");
addFunction(10.f, 0.f, dummy_func, "llTakeControls", NULL, "iii");
addFunction(10.f, 0.f, dummy_func, "llReleaseControls", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llAttachToAvatar", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llDetachFromAvatar", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llTakeCamera", NULL, "k");
addFunction(10.f, 0.f, dummy_func, "llReleaseCamera", NULL, "k");
addFunction(10.f, 0.f, dummy_func, "llGetOwner", "k", NULL);
addFunction(10.f, 2.f, dummy_func, "llInstantMessage", NULL, "ks");
addFunction(10.f, 20.f, dummy_func, "llEmail", NULL, "sss");
addFunction(10.f, 0.f, dummy_func, "llGetNextEmail", NULL, "ss");
addFunction(10.f, 0.f, dummy_func, "llGetKey", "k", NULL);
addFunction(10.f, 0.f, dummy_func, "llSetBuoyancy", NULL, "f");
addFunction(10.f, 0.f, dummy_func, "llSetHoverHeight", NULL, "fif");
addFunction(10.f, 0.f, dummy_func, "llStopHover", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llMinEventDelay", NULL, "f");
addFunction(10.f, 0.f, dummy_func, "llSoundPreload", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llRotLookAt", NULL, "qff");
addFunction(10.f, 0.f, dummy_func, "llStringLength", "i", "s");
addFunction(10.f, 0.f, dummy_func, "llStartAnimation", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llStopAnimation", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llPointAt", NULL, "v");
addFunction(10.f, 0.f, dummy_func, "llStopPointAt", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llTargetOmega", NULL, "vff");
addFunction(10.f, 0.f, dummy_func, "llGetStartParameter", "i", NULL);
addFunction(10.f, 0.f, dummy_func, "llGodLikeRezObject", NULL, "kv", TRUE);
addFunction(10.f, 0.f, dummy_func, "llRequestPermissions", NULL, "ki");
addFunction(10.f, 0.f, dummy_func, "llGetPermissionsKey", "k", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetPermissions", "i", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetLinkNumber", "i", NULL);
addFunction(10.f, 0.f, dummy_func, "llSetLinkColor", NULL, "ivi");
addFunction(10.f, 1.f, dummy_func, "llCreateLink", NULL, "ki");
addFunction(10.f, 0.f, dummy_func, "llBreakLink", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llBreakAllLinks", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llGetLinkKey", "k", "i");
addFunction(10.f, 0.f, dummy_func, "llGetLinkName", "s", "i");
addFunction(10.f, 0.f, dummy_func, "llGetInventoryNumber", "i", "i");
addFunction(10.f, 0.f, dummy_func, "llGetInventoryName", "s", "ii");
addFunction(10.f, 0.f, dummy_func, "llSetScriptState", NULL, "si");
addFunction(10.f, 0.f, dummy_func, "llGetEnergy", "f", NULL);
addFunction(10.f, 0.f, dummy_func, "llGiveInventory", NULL, "ks");
addFunction(10.f, 0.f, dummy_func, "llRemoveInventory", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llSetText", NULL, "svf");
addFunction(10.f, 0.f, dummy_func, "llWater", "f", "v");
addFunction(10.f, 0.f, dummy_func, "llPassTouches", NULL, "i");
addFunction(10.f, 0.1f, dummy_func, "llRequestAgentData", "k", "ki");
addFunction(10.f, 1.f, dummy_func, "llRequestInventoryData", "k", "s");
addFunction(10.f, 0.f, dummy_func, "llSetDamage", NULL, "f");
addFunction(100.f, 5.f, dummy_func, "llTeleportAgentHome", NULL, "k");
addFunction(10.f, 0.f, dummy_func, "llModifyLand", NULL, "ii");
addFunction(10.f, 0.f, dummy_func, "llCollisionSound", NULL, "sf");
addFunction(10.f, 0.f, dummy_func, "llCollisionSprite", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llGetAnimation", "s", "k");
addFunction(10.f, 0.f, dummy_func, "llResetScript", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llMessageLinked", NULL, "iisk");
addFunction(10.f, 0.f, dummy_func, "llPushObject", NULL, "kvvi");
addFunction(10.f, 0.f, dummy_func, "llPassCollisions", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llGetScriptName", "s", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetNumberOfSides", "i", NULL);

addFunction(10.f, 0.f, dummy_func, "llGetInventoryKey", "k", "s");
addFunction(10.f, 0.f, dummy_func, "llAllowInventoryDrop", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llGetSunDirection", "v", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetTextureOffset", "v", "i");
addFunction(10.f, 0.f, dummy_func, "llGetTextureScale", "v", "i");
addFunction(10.f, 0.f, dummy_func, "llGetTextureRot", "f", "i");
addFunction(10.f, 0.f, dummy_func, "llGetOwnerKey", "k", "k");
addFunction(10.f, 0.f, dummy_func, "llGetCenterOfMass", "v", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetRegionCorner", "v", NULL);

addFunction(10.f, 0.f, dummy_func, "llGetObjectName", "s", NULL);
addFunction(10.f, 0.f, dummy_func, "llSetObjectName", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llGetDate", "s", NULL);
addFunction(10.f, 0.f, dummy_func, "llEdgeOfWorld", "i", "vv");
addFunction(10.f, 0.f, dummy_func, "llGetAgentInfo", "i", "k");
addFunction(10.f, 0.1f, dummy_func, "llAdjustSoundVolume", NULL, "f");
addFunction(10.f, 0.f, dummy_func, "llSetSoundQueueing", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llSetSoundRadius", NULL, "f");
addFunction(10.f, 0.f, dummy_func, "llKey2Name", "s", "k");
addFunction(10.f, 0.f, dummy_func, "llSetTextureAnim", NULL, "iiiifff");
addFunction(10.f, 0.f, dummy_func, "llTriggerSoundLimited", NULL, "sfvv");
addFunction(10.f, 0.f, dummy_func, "llEjectFromLand", NULL, "k");

addFunction(10.f, 0.f, dummy_func, "llOverMyLand", "i", "k");
addFunction(10.f, 0.f, dummy_func, "llGetLandOwnerAt", "k", "v");
addFunction(10.f, 0.1f, dummy_func, "llGetNotecardLine", "k", "si");
addFunction(10.f, 0.f, dummy_func, "llGetAgentSize", "v", "k");
addFunction(10.f, 0.f, dummy_func, "llSameGroup", "i", "k");
addFunction(10.f, 0.f, dummy_func, "llUnSit", NULL, "k");
addFunction(10.f, 0.f, dummy_func, "llGroundSlope", "v", "v");
addFunction(10.f, 0.f, dummy_func, "llGroundNormal", "v", "v");
addFunction(10.f, 0.f, dummy_func, "llGroundContour", "v", "v");
addFunction(10.f, 0.f, dummy_func, "llGetAttached", "i", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetFreeMemory", "i", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetRegionName", "s", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetRegionTimeDilation", "f", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetRegionFPS", "f", NULL);

addFunction(10.f, 0.f, dummy_func, "llParticleSystem", NULL, "l");
addFunction(10.f, 0.f, dummy_func, "llGroundRepel", NULL, "fif");
addFunction(10.f, 3.f, dummy_func, "llGiveInventoryList", NULL, "ksl");

// script calls for vehicle action
addFunction(10.f, 0.f, dummy_func, "llSetVehicleType", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llSetVehicleFloatParam", NULL, "if");
addFunction(10.f, 0.f, dummy_func, "llSetVehicleVectorParam", NULL, "iv");
addFunction(10.f, 0.f, dummy_func, "llSetVehicleRotationParam", NULL, "iq");
addFunction(10.f, 0.f, dummy_func, "llSetVehicleFlags", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llRemoveVehicleFlags", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llSitTarget", NULL, "vq");
addFunction(10.f, 0.f, dummy_func, "llAvatarOnSitTarget", "k", NULL);
addFunction(10.f, 0.1f, dummy_func, "llAddToLandPassList", NULL, "kf");
addFunction(10.f, 0.f, dummy_func, "llSetTouchText", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llSetSitText", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llSetCameraEyeOffset", NULL, "v");
addFunction(10.f, 0.f, dummy_func, "llSetCameraAtOffset", NULL, "v");

addFunction(10.f, 0.f, dummy_func, "llScriptDanger", "i", "v");
addFunction(10.f, 1.f, dummy_func, "llDialog", NULL, "ksli");
addFunction(10.f, 0.f, dummy_func, "llVolumeDetect", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llResetOtherScript", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llGetScriptState", "i", "s");
addFunction(10.f, 3.f, dummy_func, "llRemoteLoadScript", NULL, "ksii");

addFunction(10.f, 0.2f, dummy_func, "llSetRemoteScriptAccessPin", NULL, "i");
addFunction(10.f, 3.f, dummy_func, "llRemoteLoadScriptPin", NULL, "ksiii");

addFunction(10.f, 1.f, dummy_func, "llOpenRemoteDataChannel", NULL, NULL);
addFunction(10.f, 3.f, dummy_func, "llSendRemoteData", "k", "ksis");
addFunction(10.f, 3.f, dummy_func, "llRemoteDataReply", NULL, "kksi");
addFunction(10.f, 1.f, dummy_func, "llCloseRemoteDataChannel", NULL, "k");

addFunction(10.f, 0.2f, dummy_func, "llSetPrimitiveParams", NULL, "l");
addFunction(10.f, 0.f, dummy_func, "llRemoteDataSetRegion", NULL, NULL);
addFunction(10.f, 0.f, dummy_func, "llGetAnimationList", "l", "k");
addFunction(10.f, 2.f, dummy_func, "llSetParcelMusicURL", NULL, "s");

addFunction(10.f, 0.f, dummy_func, "llGetRootPosition", "v", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetRootRotation", "q", NULL);

addFunction(10.f, 0.f, dummy_func, "llGetObjectDesc", "s", NULL);
addFunction(10.f, 0.f, dummy_func, "llSetObjectDesc", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llGetCreator", "k", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetTimestamp", "s", NULL);
addFunction(10.f, 0.f, dummy_func, "llSetLinkAlpha", NULL, "ifi");
addFunction(10.f, 0.f, dummy_func, "llGetNumberOfPrims", "i", NULL);
addFunction(10.f, 0.1f, dummy_func, "llGetNumberOfNotecardLines", "k", "s");

addFunction(10.f, 0.f, dummy_func, "llGetBoundingBox", "l", "k");
addFunction(10.f, 0.f, dummy_func, "llGetGeometricCenter", "v", NULL);
addFunction(10.f, 0.2f, dummy_func, "llGetPrimitiveParams", "l", "l");
addFunction(10.f, 0.f, dummy_func, "llGetGMTclock", "f", "");
addFunction(10.f, 10.f, dummy_func, "llGetSimulatorHostname", "s", "");


addFunction(10.f, 0.2f, dummy_func, "llSetLocalRot", NULL, "q");

addFunction(200.f, 0.1f, dummy_func, "llRezAtRoot", NULL, "svvqi");

addFunction(10.f, 0.f, dummy_func, "llGetObjectPermMask", "i", "i", FALSE);
addFunction(10.f, 0.f, dummy_func, "llSetObjectPermMask", NULL, "ii", TRUE);

addFunction(10.f, 0.f, dummy_func, "llGetInventoryPermMask", "i", "si", FALSE);
addFunction(10.f, 0.f, dummy_func, "llSetInventoryPermMask", NULL, "sii", TRUE);
addFunction(10.f, 0.f, dummy_func, "llGetInventoryCreator", "k", "s", FALSE);
addFunction(10.f, 0.f, dummy_func, "llOwnerSay", NULL, "s");
addFunction(10.f, 1.f, dummy_func, "llRequestSimulatorData", "k", "si");
addFunction(10.f, 0.f, dummy_func, "llForceMouselook", NULL, "i");
addFunction(10.f, 0.f, dummy_func, "llGetObjectMass", "f", "k");
addFunction(10.f, 0.f, dummy_func, "llListReplaceList", "l", "llii");
addFunction(10.f, 10.f, dummy_func, "llLoadURL", NULL, "kss");

addFunction(10.f, 2.f, dummy_func, "llParcelMediaCommandList", NULL, "l");
addFunction(10.f, 2.f, dummy_func, "llParcelMediaQuery", "l", "l");

addFunction(10.f, 0.f, dummy_func, "llGetInventoryType", "i", "s");
addFunction(10.f, 0.f, dummy_func, "llSetPayPrice", NULL, "il");
addFunction(10.f, 0.f, dummy_func, "llGetCameraPos", "v", "");
addFunction(10.f, 0.f, dummy_func, "llGetCameraRot", "q", "");

addFunction(10.f, 20.f, dummy_func, "llSetPrimURL", NULL, "s");
addFunction(10.f, 20.f, dummy_func, "llRefreshPrimURL", NULL, "");
addFunction(10.f, 0.f, dummy_func, "llEscapeURL", "s", "s");
addFunction(10.f, 0.f, dummy_func, "llUnescapeURL", "s", "s");

addFunction(10.f, 1.f, dummy_func, "llMapDestination", NULL, "svv");
addFunction(10.f, 0.1f, dummy_func, "llAddToLandBanList", NULL, "kf");
addFunction(10.f, 0.1f, dummy_func, "llRemoveFromLandPassList", NULL, "k");
addFunction(10.f, 0.1f, dummy_func, "llRemoveFromLandBanList", NULL, "k");

addFunction(10.f, 0.f, dummy_func, "llSetCameraParams", NULL, "l");
addFunction(10.f, 0.f, dummy_func, "llClearCameraParams", NULL, NULL);

addFunction(10.f, 0.f, dummy_func, "llListStatistics", "f", "il");
addFunction(10.f, 0.f, dummy_func, "llGetUnixTime", "i", NULL);
addFunction(10.f, 0.f, dummy_func, "llGetParcelFlags", "i", "v");
addFunction(10.f, 0.f, dummy_func, "llGetRegionFlags", "i", NULL);

addFunction(10.f, 0.f, dummy_func, "llHTTPRequest", "k", "sls");

addFunction(10.f, 0.1f, dummy_func, "llResetLandBanList", NULL, NULL);
addFunction(10.f, 0.1f, dummy_func, "llResetLandPassList", NULL, NULL);

addFunction(10.f, 0.f, dummy_func, "llGetObjectPrimCount", "i", "k");
addFunction(10.f, 2.0f, dummy_func, "llGetParcelPrimOwners", "l", "v");
addFunction(10.f, 0.f, dummy_func, "llGetParcelPrimCount", "i", "vii");
addFunction(10.f, 0.f, dummy_func, "llGetParcelMaxPrims", "i", "vi");
addFunction(10.f, 0.f, dummy_func, "llGetParcelDetails", "l", "vl");


addFunction(10.f, 0.2f, dummy_func, "llSetLinkPrimitiveParams", NULL, "il");
addFunction(10.f, 0.2f, dummy_func, "llSetLinkTexture", NULL, "isi");


addFunction(10.f, 0.f, dummy_func, "llStringTrim", "s", "si");
addFunction(10.f, 0.f, dummy_func, "llRegionSay", NULL, "is");
addFunction(10.f, 0.f, dummy_func, "llGetObjectDetails", "l", "kl");
addFunction(10.f, 0.f, dummy_func, "llSetClickAction", NULL, "i");

addFunction(10.f, 0.f, dummy_func, "llGetRegionAgentCount", "i", NULL);
addFunction(10.f, 1.f, dummy_func, "llTextBox", NULL, "ksi");
addFunction(10.f, 0.f, dummy_func, "llGetAgentLanguage", "s", "k");
addFunction(10.f, 0.f, dummy_func, "llDetectedTouchUV", "v", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedTouchFace", "i", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedTouchPos", "v", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedTouchNormal", "v", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedTouchBinormal", "v", "i");
addFunction(10.f, 0.f, dummy_func, "llDetectedTouchST", "v", "i");


addFunction(10.f, 0.f, dummy_func, "llGetFreeURLs", "i", NULL);
addFunction(10.f, 0.f, dummy_func, "llRequestURL", "k", NULL);
addFunction(10.f, 0.f, dummy_func, "llRequestSecureURL", "k", NULL);
addFunction(10.f, 0.f, dummy_func, "llReleaseURL", NULL, "s");
addFunction(10.f, 0.f, dummy_func, "llHTTPResponse", NULL, "kis");
addFunction(10.f, 0.f, dummy_func, "llGetHTTPHeader", "s", "ks");

// Prim media (see lscript_prim_media.h)
addFunction(10.f, 1.0f, dummy_func, "llSetPrimMediaParams", "i", "il");
addFunction(10.f, 1.0f, dummy_func, "llGetPrimMediaParams", "l", "il");
addFunction(10.f, 1.0f, dummy_func, "llClearPrimMedia", "i", "i");
addFunction(10.f, 0.f, dummy_func, "llSetLinkPrimitiveParamsFast", NULL, "il");
addFunction(10.f, 0.f, dummy_func, "llGetLinkPrimitiveParams", "l", "il");
addFunction(10.f, 0.f, dummy_func, "llLinkParticleSystem", NULL, "il");
addFunction(10.f, 0.f, dummy_func, "llSetLinkTextureAnim", NULL, "iiiiifff");

addFunction(10.f, 0.f, dummy_func, "llGetLinkNumberOfSides", "i", "i");

// IDEVO Name lookup calls, see lscript_avatar_names.h
addFunction(10.f, 0.f, dummy_func, "llGetUsername", "s", "k");
addFunction(10.f, 0.f, dummy_func, "llRequestUsername", "k", "k");
addFunction(10.f, 0.f, dummy_func, "llGetDisplayName", "s", "k");
addFunction(10.f, 0.f, dummy_func, "llRequestDisplayName", "k", "k");

addFunction(10.f, 0.f, dummy_func, "llGetEnv", "s", "s");
addFunction(10.f, 0.f, dummy_func, "llRegionSayTo", NULL, "kis");
#endif

#endif //GUARD_LSL_RUNTIME_LIBRARY_FUNCTIONS_HH_INCLUDED
