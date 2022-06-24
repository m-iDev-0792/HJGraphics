# specific working list
- [x] add render tag for sub meshes
- [x] submesh sorting in MeshRenderSystem
- [x] add ECS support for shadow pass
- [x] test ECS model loading
- [ ] save shader code and check uniform name when setting uniform variable
- [ ] light prefab, [Bug]RenderSystem will handle light prefab as well [Refactor] remove isLightVolumeDirty, and getter setter methods
- [ ] shared tag for SubMesh, skip unshared submesh when commit vertex data
- [ ] solve assimp model import flag conflict

# todo list
- [x] mesh component, light component, camera component?
- [ ] think how to transform HJGraphics to ECS architecture
- [x] Prefab for sphere, cube, plane

- [x] prefab, virtual bool Prefab::instantiate(ECSScene* _scene, EntityID _entityID)=0;
- [x] ECSScene, bool ECSScene::instantiatePrefab(Prefab _prefab);

# main issues
- [ ] cast entity's relative position when attaching it to another
- [ ] setting absolute and relative position