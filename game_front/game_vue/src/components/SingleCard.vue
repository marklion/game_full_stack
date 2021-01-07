<template>
<div class="single_card">
    <el-image :src="card_link" fit="fill" class="single_card">
        <el-image slot="error" :src="url" class="single_card"></el-image>
    </el-image>
</div>
</template>

<script>
export default {
    name: 'SingleCard',
    props: {
        card_link: String
    },
    data: function() {
        return  {
            url:'http://www.d8sis.cn/game_resource/cards_imgs/little_joker.jpg'
        }
    }
}
</script>

<style lang="scss" scoped>
.single_card {
    height: 100%;
}
</style>
