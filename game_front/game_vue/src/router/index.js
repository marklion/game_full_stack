import Vue from 'vue'
import VueRouter from 'vue-router'
import Home from '../views/Home.vue'
import Table from '../views/Table.vue'
import WechatLogin from '../views/WechatLogin.vue'
import QQlogin from '../views/QQlogin.vue'

Vue.use(VueRouter)

const routes = [
  {
    path: '/',
    name: 'Home',
    component: Home
  },
  {
    path: '/wechatlogin',
    name: 'WechatLogin',
    component: WechatLogin
  },
  {
    path: '/qqlogin',
    name: 'QQlogin',
    component: QQlogin 
  },
  {
    path:'/table/:table_no',
    name: 'Table',
    component: Table
  },
]

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes
})

export default router
