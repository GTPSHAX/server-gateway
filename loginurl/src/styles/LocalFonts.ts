import localFont from "next/font/local";

export const pixel_font7 = localFont({
    src: '../../public/fonts/pixel-font7.ttf',
    variable: '--pixel-font7',
    display: 'swap',
});

export const pixelic_war = localFont({
    src: [
        {
            path: '../../public/fonts/pixelic-war.ttf',
            weight: '400',
            style: 'normal',
        },
        {
            path: '../../public/fonts/pixelic-war.otf',
            weight: '400',
            style: 'normal',
        }
    ],
    variable: '--pixelic-war',
    display: 'swap',
});

