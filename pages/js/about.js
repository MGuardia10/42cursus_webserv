// Select all items in about section
const items = document.querySelectorAll('.questions .question-container');

items.forEach((item) => {
    item.addEventListener('click', (e) => {
        e.currentTarget.classList.toggle('active');

        const respuesta = item.querySelector('.answer');
        const alturaRealRespuesta = respuesta.scrollHeight;

        if(!respuesta.style.maxHeight){
            //put a value when maxHeight is empty.
            respuesta.style.maxHeight = alturaRealRespuesta + 'px';
        } else {
            respuesta.style.maxHeight = null;
        }

        //Close other items
        items.forEach((elem) => {
            if(item !== elem){
                elem.classList.remove('active');
                elem.querySelector('.answer').style.maxHeight = null;  
            }
        });
    });
});