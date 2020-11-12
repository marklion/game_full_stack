<template>
    <div>

    </div>
</template>

<script>
export default {
    name:'WechatLogin',
    beforeMount: function() {
        var vue_this = this;
        if (this.$route.query.code)
        {
            this.axios.post('/game_rest/wechat_login', {code:this.$route.query.code}).then(
                function(resp) {
                    vue_this.$cookies.set("ssid", resp.data.result);
                    window.location.replace('/');
                }
            ).catch(function(err) {
                console.log(err);
            });
        }
    },
}
</script>

<style>

</style>