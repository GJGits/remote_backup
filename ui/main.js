const { menubar } = require('menubar');

const mb = menubar({icon:"../imgs/remote-icon.png", tooltip:"tooltip-name"});

mb.on('ready', () => {
  console.log('app is ready');
  // your app code here
});