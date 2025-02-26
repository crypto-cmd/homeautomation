/**
 * plugins/index.js
 *
 * Automatically included in `./src/main.js`
 */

// Plugins
import vuetify from './vuetify'
import { createPinia } from 'pinia'
import piniaPluginPersistedstate from "pinia-plugin-persistedstate";
import toastify from 'vue-toastify';

import router from '../router'

const pinia = createPinia();
pinia.use(piniaPluginPersistedstate);

import 'vue-toastify/index.css';
// theme styles
import 'vue-toastify/themes/dark.css';



export function registerPlugins (app) {
  app
    .use(vuetify)
    .use(router)
    .use(pinia)
    .use(toastify, { customNotifications: {
      authenticationError: {
          body: 'Authentication error',
          // ... rest of the toast options here
      }
  } });

}
