#import "@preview/physica:0.9.5": *

#let template(title: "Title", class: "Class", doc) = {
  set par(leading: 0.55em, justify: true)
  set text(font: "New Computer Modern")
  show raw: set text(font: "New Computer Modern Mono")
  show heading: set block(above: 1.4em, below: 1em)

  show link: it => {
    set text(blue)
    if type(it.dest) != str {
      it
    }
    else {
      underline(it)
    }
  }
  show ref: it => {
    let eq = math.equation
    let el = it.element
    if el != none and el.func() == eq {
      // Override equation references.
      link(el.location(),numbering(
        el.numbering,
        ..counter(eq).at(el.location())
      ))
    } else {
      // Other references as usual.
      it
    }
  }
  set math.equation(numbering: "(1)") 
  
  show math.equation: it => {
    if it.block and not it.has("label") [
      #counter(math.equation).update(v => v - 1)
      #math.equation(it.body, block: true, numbering: none)#label(" ")
    ] else {
      it
    }  
  } 
  
  set page(numbering: "1")
  counter(page).update(1) 
  
  set page(header: [
    #title
    #h(1fr)
    #class
    #line(length: 100%, stroke: 0.8pt) // added line, remove if you dont like  
  ])
  
  set heading(offset: 0)
  align(center, [= #title])

  set heading(numbering: "1.1.")
  doc
}