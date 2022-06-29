# today
1. create a draw mesh function for light shadow system
and complete the light shading system
2. test our ecs light system
   - construct a scene that only contain ecs entities(replace floor, pbr spheres with ecs system)
   - test our light system
3. 
# specific working list
- [ ] save shader code and check uniform name when setting uniform variable
- [x] light prefab, [Bug]RenderSystem will handle light prefab as well [Refactor] remove isLightVolumeDirty, and getter setter methods
- [ ] shared tag for SubMesh, skip unshared submesh when commit vertex data
- [ ] solve assimp model import flag conflict

# todo list
- [ ] CustomMeshComponent
- [ ] GizmoComponent
- [ ] IBLComponent IBLPrefab

# main issues
- [ ] cast entity's relative position when attaching it to another
- [ ] setting absolute and relative position