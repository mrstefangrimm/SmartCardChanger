import { describe, expect, it } from 'vitest';
import { ThreeObject } from './threeobject.model';

describe('ThreeObject', () => {
  it('should create an instance', () => {
    expect(new ThreeObject()).toBeTruthy();
  });
});

