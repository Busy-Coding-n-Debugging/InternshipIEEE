
const scene=new THREE.Scene();
const camera=new THREE.PerspectiveCamera(75,innerWidth/innerHeight,0.1,3000);
camera.position.z=500;

const renderer=new THREE.WebGLRenderer({alpha:true,antialias:true});
renderer.setSize(innerWidth,innerHeight);
document.getElementById('space-bg').appendChild(renderer.domElement);

const count=18000;
const geo=new THREE.BufferGeometry();
const positions=[];
const original=[];

for(let i=0;i<count;i++){
 const x=(Math.random()-0.5)*2000;
 const y=(Math.random()-0.5)*2000;
 const z=(Math.random()-0.5)*1200;

 positions.push(x,y,z);
 original.push(x,y,z);
}

geo.setAttribute('position',new THREE.Float32BufferAttribute(positions,3));

const stars=new THREE.Points(
 geo,
 new THREE.PointsMaterial({
   color:0xffffff,
   size:1.2,
   transparent:true,
   opacity:0.85
 })
);

scene.add(stars);

let mx=9999,my=9999;

addEventListener('mousemove',e=>{
 mx=(e.clientX/innerWidth-0.5)*800;
 my=-(e.clientY/innerHeight-0.5)*500;
});

function animate(){
 requestAnimationFrame(animate);

 stars.rotation.y += 0.00015;

 const p=geo.attributes.position.array;

 for(let i=0;i<p.length;i+=3){

   const ox=original[i];
   const oy=original[i+1];
   const oz=original[i+2];

   const dx=p[i]-mx;
   const dy=p[i+1]-my;

   const d=Math.sqrt(dx*dx+dy*dy);

   if(d<140){
      const f=(140-d)/140;
      p[i]+=dx*f*0.08;
      p[i+1]+=dy*f*0.08;
   }

   p[i]+=(ox-p[i])*0.015;
   p[i+1]+=(oy-p[i+1])*0.015;
   p[i+2]+=(oz-p[i+2])*0.015;
 }

 geo.attributes.position.needsUpdate=true;
 renderer.render(scene,camera);
}

animate();

addEventListener('resize',()=>{
 camera.aspect=innerWidth/innerHeight;
 camera.updateProjectionMatrix();
 renderer.setSize(innerWidth,innerHeight);
});
