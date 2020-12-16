import { Directive, HostListener, ElementRef } from '@angular/core';

@Directive({
    selector: '[headerButtonHover]'
})
export class HeaderButtonHoverDirective {

    constructor(private element: ElementRef) { }

    @HostListener('mouseenter') onMouseEnter() {
        this.element.nativeElement.style.background = 'gray';
    }

    @HostListener('mouseleave') onMouseLeave() {
        this.element.nativeElement.style.background = '#78c557';
    }
}