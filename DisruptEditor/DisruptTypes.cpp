#include "DisruptTypes.h"

#include "Serialization.h"
#include "SDL.h"
#include "FileHandler.h"
#include "IBinaryArchive.h"

void CResourceContainer::read(IBinaryArchive& fp) {
	fp.serialize(type.id);
	fp.serialize(file.id);
}

void CResourceContainer::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(file);
}

void CArchetypeResource::read(IBinaryArchive& fp) {
	fp.serialize(file.id);
	if(file.id != -1)
		fp.serialize(type.id);
}

void CArchetypeResource::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(file);
}

void CGeometryResource::read(IBinaryArchive& fp) {
	fp.serialize(file.id);
	if (file.id != -1)
		fp.serialize(type.id);
}

void CGeometryResource::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(file);
	REGISTER_MEMBER(type);
}

void CMaterialSlotsMap::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	uint32_t count;
	fp.serialize(count);
	for (uint32_t i = 0; i < count; ++i) {
		auto &it = slots.emplace_back();
		fp.serialize(it.first.id);
		it.second.read(fp);
	}
}

void CMaterialSlotsMap::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(slots);
}

void CMaterialSlotValue::read(IBinaryArchive& fp) {
	res.read(fp);
	fp.serialize(type.id);
}

void CMaterialSlotValue::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(res);
	REGISTER_MEMBER(type);
}

void CMaterialResource::read(IBinaryArchive& fp) {
	fp.serialize(file.id);
	if(file.id != -1)
		fp.serialize(type.id);
}

void CMaterialResource::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(file);
	REGISTER_MEMBER(type);
}

void CProjectedDecalInfo::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
	fp.serialize(unk8);
}

void CProjectedDecalInfo::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(unk8);
}

void SInstanceRange::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
	fp.serialize(unk8);
	fp.serialize(unk9);
	fp.serialize(unk10);
	fp.serialize(unk11);
	fp.serialize(unk12);
}

void SInstanceRange::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(unk8);
	REGISTER_MEMBER(unk9);
	REGISTER_MEMBER(unk10);
	REGISTER_MEMBER(unk11);
	REGISTER_MEMBER(unk12);
}

void CParticlesSystemParamResource::read(IBinaryArchive& fp) {
	fp.serialize(file.id);
	if (file.id != -1)
		fp.serialize(type.id);
}

void CParticlesSystemParamResource::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(file);
	REGISTER_MEMBER(type);
}

void CParticlesSystemHdl::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
}

void CParticlesSystemHdl::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(unk1);
}

void CDynamicLightObject::read(IBinaryArchive& fp) {
	fp.serialize(bEnabled);
	fp.serialize(unk2);
	fp.serialize(vectorExtraPositionOffset);
	fp.serialize(fExtraDoubleSpecularOffset);
	settings.read(fp);
}

void CDynamicLightObject::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(bEnabled);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(vectorExtraPositionOffset);
	REGISTER_MEMBER(fExtraDoubleSpecularOffset);
	REGISTER_MEMBER(settings);
}

void CDynamicLightSettings::read(IBinaryArchive& fp) {
	fp.serialize(type);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
	fp.serialize(unk8);
	targets.read(fp);
	fp.serialize(unk9);
	fp.serialize(unk10);
	fp.serialize(unk11);
	fp.serialize(unk12);
	fp.serialize(unk13);
	fp.serialize(unk14);
	fp.serialize(unk15);
	fp.serialize(unk16);
	fp.serialize(unk17);
	fp.serialize(unk18);
	fp.serialize(unk19);
	fp.serialize(unk20);
	fp.serialize(unk21);
	fp.serialize(unk22);
	fp.serialize(unk23);
	fp.serialize(unk24);
	fp.serialize(unk25);
	fp.serialize(unk26);
	fp.serialize(unk27);
	fp.serialize(unk28);
	fp.serialize(unk29);
	fp.serialize(unk30);
	fp.serialize(unk31);
	fp.serialize(unk32);
	fp.serialize(unk33);
	fp.serialize(unk34);
	fp.serialize(unk35);
	fp.serialize(unk36);
	fp.serialize(unk37);
	fp.serialize(unk38);
	fp.serialize(unk39.id);
	fp.serialize(unk40.id);
	fp.serialize(unk41);
	fp.serialize(unk42);
	fp.serialize(unk43);
	fp.serialize(unk44);
	fp.serialize(unk45);
	fp.serialize(unk46);

	SDL_Log("File1=%s", unk39.getReverseFilename().c_str());
	SDL_Log("File2=%s", unk40.getReverseFilename().c_str());
	SDL_Log("Tell %u", fp.tell());

	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVectorExternal<CSceneLightClipPlane, NoLock, ndVectorPropertiesWrapper<ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>, ndVectorAllowExternalCopyProperties>>]
	fp.serializeNdVectorExternal(clipPlanes);

	fp.serialize(unk47);
	fp.serialize(unk48);
}

void CDynamicLightSettings::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(unk8);
	REGISTER_MEMBER(targets);
	REGISTER_MEMBER(unk9);
	REGISTER_MEMBER(unk10);
	REGISTER_MEMBER(unk11);
	REGISTER_MEMBER(unk12);
	REGISTER_MEMBER(unk13);
	REGISTER_MEMBER(unk14);
	REGISTER_MEMBER(unk15);
	REGISTER_MEMBER(unk16);
	REGISTER_MEMBER(unk17);
	REGISTER_MEMBER(unk18);
	REGISTER_MEMBER(unk19);
	REGISTER_MEMBER(unk20);
	REGISTER_MEMBER(unk21);
	REGISTER_MEMBER(unk22);
	REGISTER_MEMBER(unk23);
	REGISTER_MEMBER(unk24);
	REGISTER_MEMBER(unk25);
	REGISTER_MEMBER(unk26);
	REGISTER_MEMBER(unk27);
	REGISTER_MEMBER(unk28);
	REGISTER_MEMBER(unk29);
	REGISTER_MEMBER(unk30);
	REGISTER_MEMBER(unk31);
	REGISTER_MEMBER(unk32);
	REGISTER_MEMBER(unk33);
	REGISTER_MEMBER(unk34);
	REGISTER_MEMBER(unk35);
	REGISTER_MEMBER(unk36);
	REGISTER_MEMBER(unk37);
	REGISTER_MEMBER(unk38);
	REGISTER_MEMBER(unk39);
	REGISTER_MEMBER(unk40);
	REGISTER_MEMBER(unk41);
	REGISTER_MEMBER(unk42);
	REGISTER_MEMBER(unk43);
	REGISTER_MEMBER(unk44);
	REGISTER_MEMBER(unk45);
	REGISTER_MEMBER(unk46);
	REGISTER_MEMBER(clipPlanes);
	REGISTER_MEMBER(unk47);
	REGISTER_MEMBER(unk48);
}

void CSceneLightTargets::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(bGeometry);
	fp.serialize(bParticles);
	fp.serialize(bParticlesShadow);
	fp.serialize(bParticlesProjectedTexture);
	fp.serialize(bWater);
	fp.serialize(bGlobalIllumination);
}

void CSceneLightTargets::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(bGeometry);
	REGISTER_MEMBER(bParticles);
	REGISTER_MEMBER(bParticlesShadow);
	REGISTER_MEMBER(bParticlesProjectedTexture);
	REGISTER_MEMBER(bWater);
	REGISTER_MEMBER(bGlobalIllumination);
}

void CSceneLightClipPlane::read(IBinaryArchive& fp) {
	fp.serialize(angYaw);
	fp.serialize(angPitch);
	fp.serialize(fDistance);
	fp.serialize(fFadeDistance);
	fp.serialize(bOccludeBounce);
}

void CSceneLightClipPlane::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(angYaw);
	REGISTER_MEMBER(angPitch);
	REGISTER_MEMBER(fDistance);
	REGISTER_MEMBER(fFadeDistance);
	REGISTER_MEMBER(bOccludeBounce);
}

void CSceneLight::read(IBinaryArchive& fp) {
	uint32_t count;
	fp.serialize(count);

	uint32_t CSceneLightClipPlaneType;
	fp.serialize(CSceneLightClipPlaneType);
	SDL_assert_release(CSceneLightClipPlaneType == 2461405956);

	uint32_t u3, countAgain;
	fp.serialize(u3);
	fp.serialize(countAgain);

	SDL_Log("Tell: %u", fp.tell());

	clipPlanes.resize(count);
	for (uint32_t i = 0; i < count; ++i)
		clipPlanes[i].read(fp);

	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);

	SDL_Log("Tell2: %u", fp.tell());
}

void CSceneLight::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(clipPlanes);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
}

void CBatchedInstanceID::read(IBinaryArchive& fp) {
	fp.serialize(id);
}

void CBatchedInstanceID::registerMembers(MemberStructure& ms) {
	ms.registerMember(NULL, id);
}

void CTextureResource::read(IBinaryArchive& fp) {
	fp.serialize(file.id);
	if (file.id != -1)
		fp.serialize(type.id);
}

void CTextureResource::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(file);
	REGISTER_MEMBER(type);
}

void CLightEffectObject::read(IBinaryArchive& fp) {
	fileTexture.read(fp);
	fp.serialize(bEnable);
	fp.serialize(unk2);
	effect.read(fp);
}

void CLightEffectObject::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(fileTexture);
	REGISTER_MEMBER(bEnable);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(effect);
}

void CSceneLightEffect::read(IBinaryArchive& fp) {
	SDL_Log("Tell: %u", fp.tell());

	//void SerializeMember<T1>(IBinaryArchive &, T1 &) [with T1=ndVector<CLightEffectFlareElement, NoLock, ndVectorTracker<(unsigned long)18, (unsigned long)4, (unsigned long)9>, false>]
	fp.serializeNdVectorExternal(flares);

	fp.serialize(effectType);
	fp.serialize(sourceType);
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);
	fp.serialize(unk6);
	fp.serialize(unk7);
	fp.serialize(unk8);
	fp.serialize(unk9);
	fp.serialize(unk10);
	fp.serialize(unk11);
	fp.serialize(unk12);
	fp.serialize(unk13);
	fp.serialize(unk14);
	fp.serialize(unk15);
	fp.serialize(unk16);
	fp.serialize(unk17);
	fp.serialize(unk18);
	fp.serialize(unk19);
	fp.serialize(unk20);
	fp.serialize(unk21);
	fp.serialize(unk22);
	fp.serialize(unk23);
	fp.serialize(unk24);
	fp.serialize(unk25);
	fp.serialize(unk26);
	fp.serialize(unk27);
	fp.serialize(unk28);
	fp.serialize(unk29);
	fp.serialize(unk30);
	fp.serialize(unk31);
	fp.serialize(unk32);
	fp.serialize(ang1);
	fp.serialize(unk33);
	fp.serialize(unk34);
	fp.serialize(unk35);
	fp.serialize(unk36);
	fp.serialize(unk37);
	fp.serialize(unk38);
	fp.serialize(unk39);
	fp.serialize(unk40);
	fp.serialize(unk41);
}

void CSceneLightEffect::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(flares);
	REGISTER_MEMBER(effectType);
	REGISTER_MEMBER(sourceType);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
	REGISTER_MEMBER(unk6);
	REGISTER_MEMBER(unk7);
	REGISTER_MEMBER(unk8);
	REGISTER_MEMBER(unk9);
	REGISTER_MEMBER(unk10);
	REGISTER_MEMBER(unk11);
	REGISTER_MEMBER(unk12);
	REGISTER_MEMBER(unk13);
	REGISTER_MEMBER(unk14);
	REGISTER_MEMBER(unk15);
	REGISTER_MEMBER(unk16);
	REGISTER_MEMBER(unk17);
	REGISTER_MEMBER(unk18);
	REGISTER_MEMBER(unk19);
	REGISTER_MEMBER(unk20);
	REGISTER_MEMBER(unk21);
	REGISTER_MEMBER(unk22);
	REGISTER_MEMBER(unk23);
	REGISTER_MEMBER(unk24);
	REGISTER_MEMBER(unk25);
	REGISTER_MEMBER(unk26);
	REGISTER_MEMBER(unk27);
	REGISTER_MEMBER(unk28);
	REGISTER_MEMBER(unk29);
	REGISTER_MEMBER(unk30);
	REGISTER_MEMBER(unk31);
	REGISTER_MEMBER(unk32);
	REGISTER_MEMBER(ang1);
	REGISTER_MEMBER(unk33);
	REGISTER_MEMBER(unk34);
	REGISTER_MEMBER(unk35);
	REGISTER_MEMBER(unk36);
	REGISTER_MEMBER(unk37);
	REGISTER_MEMBER(unk38);
	REGISTER_MEMBER(unk39);
	REGISTER_MEMBER(unk40);
	REGISTER_MEMBER(unk41);
}

void CLightEffectFlareElement::read(IBinaryArchive& fp) {
	fp.serialize(fScale);
	fp.serialize(fScaleMin);
	fp.serialize(fScaleMinThreshold);
	fp.serialize(fOffset);
	fp.serialize(iTextureSliceIndex);
	fp.serialize(vectorColor);
	fp.serialize(fRotationXAmount);
	fp.serialize(fRotationYAmount);
	fp.serialize(fFadeAngle);
}

void CLightEffectFlareElement::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(fScale);
	REGISTER_MEMBER(fScaleMin);
	REGISTER_MEMBER(fScaleMinThreshold);
	REGISTER_MEMBER(fOffset);
	REGISTER_MEMBER(iTextureSliceIndex);
	REGISTER_MEMBER(vectorColor);
	REGISTER_MEMBER(fRotationXAmount);
	REGISTER_MEMBER(fRotationYAmount);
	REGISTER_MEMBER(fFadeAngle);
}

void CSceneLightEffectInstance::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
}

void CSceneLightEffectInstance::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
}

void SSecurityCameraBatchArchetypeInformation::read(IBinaryArchive& fp) {
	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serializeNdVectorExternal_pod(unk5);
}

void SSecurityCameraBatchArchetypeInformation::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);
}

void CSecurityCameraObjectBatched::read(IBinaryArchive& fp) {
	fp.serialize(has);
	if (has) {
		fp.serialize(unk1);
		fp.serialize(unk2);
		instance.read(fp);
	}
}

void CSecurityCameraObjectBatched::registerMembers(MemberStructure& ms) {
	REGISTER_MEMBER(has);
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(instance);
}
