var spine;
(function (spine) {

    var enums = [];

    var e = spine.MixBlend = {
        setup: 0,
        first: 1,
        replace: 2,
        add: 3
    };
    enums.push(e);
    
    e = spine.MixDirection = {
        mixIn: 0,
        mixOut: 1
    };
    enums.push(e);

    e = spine.TimelineType = {
        rotate: 0,
        translate: 1,
        scale: 2,
        shear: 3,
        attachment: 4,
        color: 5,
        deform: 6,
        event: 7,
        drawOrder: 8,
        ikConstraint: 9,
        transformConstraint: 10,
        pathConstraintPosition: 11,
        pathConstraintSpacing: 12,
        pathConstraintMix: 13,
        twoColor: 14
    };
    enums.push(e);

    e = spine.EventType = {
        start:0,
        interrupt: 1,
        end: 2,
        dispose: 3,
        complete: 4,
        event: 5
    };
    enums.push(e);

    e = spine.BlendMode = {
        Normal:0,
        Additive: 1,
        Multiply: 2,
        Screen: 3
    };
    enums.push(e);

    e = spine.TransformMode = {
        Normal: 0,
        OnlyTranslation: 1,
        NoRotationOrReflection: 2,
        NoScale: 3,
        NoScaleOrReflection: 4
    };
    enums.push(e);

    e = spine.PositionMode = {
        Fixed: 0,
        Percent: 1
    };
    enums.push(e);
    
    e = spine.SpacingMode = {
        Length: 0,
        Fixed: 1,
        Percent: 2
    };
    enums.push(e);

    e = spine.RotateMode = {
        Tangent: 0,
        Chain: 1,
        ChainScale: 2
    };
    enums.push(e);

    e = spine.TextureFilter = {
        Nearest: 9728,
        Linear: 9729,
        MipMap: 9987,
        MipMapNearestNearest: 9984,
        MipMapLinearNearest: 9985,
        MipMapNearestLinear: 9986,
        MipMapLinearLinear: 9987
    };
    enums.push(e);
    
    e = spine.TextureWrap = {
        MirroredRepeat: 33648,
        ClampToEdge: 33071,
        Repeat: 10497
    };
    enums.push(e);

    e = spine.AttachmentType = {
        Region: 0,
        BoundingBox: 1,
        Mesh: 2,
        LinkedMesh: 3,
        Path: 4,
        Point: 5,
        Clipping: 6
    };
    enums.push(e);

    // Support reverse keys
    enums.forEach((e) => {
        for (var key in e) {
            var value = e[key];
            e[value] = key;
        }
    });

    e = null;

})(spine || (spine = {}));
export default spine;
