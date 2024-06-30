/*
 Copyright (c) 2024 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

import { error } from '../../core/platform/debug';
import type { ConstructorType } from '../tween';
import { springScheduler } from './spring-scheduler';

interface SpringMustHaveFunctions<T> {
    clone: () => T,
    set: (o: T) => void,
}

type SpringProperty<T> = T & SpringMustHaveFunctions<T>;

type PartialSpringProperty<T> = Partial<T>;

export class Spring<T, K extends keyof ConstructorType<T>> {
    private _target: T | null;
    private _speed = 10;
    public _currentSpeed = 0;
    private _damping = 1;
    public _currentDamping = 0;
    private _goal: SpringProperty<T[K]>;
    private _goalValue: SpringProperty<T[K]> | null = null;
    public _currentValue: SpringProperty<T[K]>;
    public _springPositions: PartialSpringProperty<T[K]> = {};
    public _springGoals: PartialSpringProperty<T[K]> = {};
    public _springVelocities: PartialSpringProperty<T[K]> = {};
    public _startDisplacements: PartialSpringProperty<T[K]> = {};
    public _startVelocities: PartialSpringProperty<T[K]> = {};
    public _lastSchedule = Number.MIN_SAFE_INTEGER;

    constructor (target: T, prop: SpringProperty<T[K]>, speed: number = 10, damping: number = 1) {
        this._target = target;
        this._speed = speed;
        this._damping = damping;
        this._goal = prop;
        this._currentValue = new (prop.constructor as Constructor<SpringProperty<T[K]>>)();
        this._currentSpeed = this._speed;
        this._currentDamping = this._damping;
    }

    setPosition (newValue: SpringProperty<T[K]>): void {
        Object.assign(this._springPositions, newValue);
        this._currentValue.set(newValue);
        springScheduler.add(this as unknown as Spring<any, string>);
        this.update();
    }

    setVelocity (newValue: SpringProperty<T[K]>): void {
        Object.assign(this._springVelocities, newValue);
        springScheduler.add(this as unknown as Spring<any, string>);
    }

    addVelocity (deltaValue: SpringProperty<T[K]>): void {
        for (const k in deltaValue) {
            const delta = deltaValue[k];
            this._springVelocities[k] += delta;
        }

        springScheduler.add(this as unknown as Spring<any, string>);
    }

    update (): boolean {
        const goalValue = this._goal;
        if (goalValue === this._goalValue) {
            const damping = this._damping;
            if (damping < 0) {
                error(`invalid spring damping: ${damping}`);
            } else {
                this._currentDamping = damping;
            }

            const speed = this._speed;
            if (speed < 0) {
                error(`invalid spring speed: ${speed}`);
            } else {
                this._currentSpeed = speed;
            }

            return false;
        } else {
            this._goalValue = goalValue;
            Object.assign(this._springGoals, this._goalValue);
            springScheduler.add(this as unknown as Spring<any, string>);
        }

        return false;
    }

    destroy (): void {
        springScheduler.remove(this as unknown as Spring<any, string>);
        this._target = null;
    }
}
