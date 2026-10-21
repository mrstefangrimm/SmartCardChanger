import {beforeEach, describe, expect, it } from 'vitest';
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { Overlaybutton } from './overlaybutton';

describe('Overlaybutton', () => {
  let component: Overlaybutton;
  let fixture: ComponentFixture<Overlaybutton>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [Overlaybutton]
    })
      .compileComponents();

    fixture = TestBed.createComponent(Overlaybutton);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
