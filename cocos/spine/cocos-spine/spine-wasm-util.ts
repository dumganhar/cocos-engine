export interface SpineWasmInterface {
    spineWasmInstanceInit(): number;
    spineWasmInstanceDestroy(): number;
    queryStoreMemory(): number;
    createSkeletonObject (): number;
    setSkeletonData(objID: number, datPtr: number);
    setAnimation(objID: number, length: number, trackIndex: number, loop: boolean): boolean;
    clearTrack(objID: number, trackIndex: number): boolean;
    clearTracks(objID: number): boolean;
    setToSetupPose(objID: number): boolean;
    setTimeScale(objID: number, timeScale: number): number;
    setSkin(objID: number, length: number): number;
    updateAnimation(objID: number, dltTime: number): number;
    setMix(objID: number, start: number, fromLength: number, toLength: number, duration: number);
    updateRenderData(objID: number): number;
    getDrawOrderSize(objID: number): number;
    getSlotNameByOrder(objID: number, index: number): number;
    getBoneMatrix(objID: number, index: number): number;
    queryMemory(size: number): number;
    freeMemory(data: Uint8Array);
    setDefaultScale(objID: number, scale: number): boolean;
    setVertexEffect(objID: number, effect: number, effectType: number);
    setPremultipliedAlpha(objID: number, premultipliedAlpha: boolean);
    setColor(objID: number, r: number, g: number, b: number, a: number);
    destroyInstance(objID: number);

    retainSkeletonDataByUUID(length: number): number;
    initSkeletonData(length: number, isJosn: boolean): number;
    recordSkeletonDataUUID(length: number, datPtr: number);

    createJitterVertexEffect(x: number, y: number): number;
    updateJitterParameters(handle: number, x: number, y: number);
    createSwirlVertexEffect(radius: number, power: number, usePowerOut: boolean): number;
    updateSwirlParameters(handle: number, centerX: number, centerY: number, radius: number, angle: number);

    memory: any;
}
