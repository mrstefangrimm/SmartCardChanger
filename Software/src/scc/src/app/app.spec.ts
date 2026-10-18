import { provideZonelessChangeDetection } from '@angular/core';
import { TestBed } from '@angular/core/testing';
import { App } from './app';
import { SccService } from './scc.service';
import { SccEngine3dService } from './sccengine3d.service';

describe('App', () => {
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [App],
      providers: [
        provideZonelessChangeDetection(),
        SccService,
        {
          provide: SccEngine3dService, useValue: {
            createScene: vi.fn(),
            animate: vi.fn(),
            setTransparent: vi.fn(),
            ngOnDestroy: vi.fn(),
          }
        }
      ]
    }).compileComponents();
  });

  it('should create the app', () => {
    const fixture = TestBed.createComponent(App);
    const app = fixture.componentInstance;
    expect(app).toBeTruthy();
  });

  it('should render "Longitudinal motion:" label', () => {
    const fixture = TestBed.createComponent(App);
    const compiled = fixture.nativeElement as HTMLElement;
    const labels = Array.from(compiled.querySelectorAll('p'));
    expect(labels.some(p => p.textContent?.includes('Longitudinal motion:'))).toBe(true);
  });

  it('should render "Rotational motion:" label', () => {
    const fixture = TestBed.createComponent(App);
    const compiled = fixture.nativeElement as HTMLElement;
    const labels = Array.from(compiled.querySelectorAll('p'));
    expect(labels.some(p => p.textContent?.includes('Rotational motion:'))).toBe(true);
  });

  it('should render the longitudinal slider with correct attributes', () => {
    const fixture = TestBed.createComponent(App);
    const compiled = fixture.nativeElement as HTMLElement;
    const slider = compiled.querySelector('#Longitudinal') as HTMLInputElement;
    expect(slider).toBeTruthy();
    expect(slider.type).toBe('range');
    expect(slider.min).toBe('20');
    expect(slider.max).toBe('180');
    expect(slider.value).toBe('180');
  });

  it('should render the rotational slider with correct attributes', () => {
    const fixture = TestBed.createComponent(App);
    const compiled = fixture.nativeElement as HTMLElement;
    const slider = compiled.querySelector('#Rotational') as HTMLInputElement;
    expect(slider).toBeTruthy();
    expect(slider.type).toBe('range');
    expect(slider.min).toBe('10');
    expect(slider.max).toBe('170');
    expect(slider.value).toBe('90');
  });
});
